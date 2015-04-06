#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "connection.h"
#include "editdialog.h"
#include "urihelper.h"
#include "uriinputdialog.h"
#include "sharedialog.h"
#include "logdialog.h"
#include "settingsdialog.h"

#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMenu>
#include <QScreen>
#include <QWindow>

#ifdef Q_OS_LINUX
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusPendingCall>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //initialisation
    configHelper = new ConfigHelper(this);
    ui->connectionView->setModel(configHelper->getModel());
    ui->connectionView->resizeColumnsToContents();
    connect(configHelper, &ConfigHelper::connected, this, &MainWindow::onConnectionConnected);
    connect(configHelper, &ConfigHelper::disconnected, this, &MainWindow::onConnectionDisconnected);
    connect(ui->actionTeat_All_Latency, &QAction::triggered, configHelper, &ConfigHelper::testAllLags);

    /*
     * There is a bug on KDE Frameworks 5: https://bugs.kde.org/show_bug.cgi?id=343976
     * As a simple work around, we give up our ownership and use deleteLater() function
     * in MainWindow's destructor.
     *
     */
    systray = new QSystemTrayIcon;
    createSystemTray();

    //Move to the center of the screen
    this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());

    connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionManual, &QAction::triggered, this, &MainWindow::onAddManually);
    connect(ui->actionQR_Code, &QAction::triggered, this, &MainWindow::onAddScreenQRCode);
    connect(ui->actionQR_Code_from_File, &QAction::triggered, this, &MainWindow::onAddQRCodeFile);
    connect(ui->actionURI, &QAction::triggered, this, &MainWindow::onAddFromURI);
    connect(ui->actionDelete, &QAction::triggered, this, &MainWindow::onDelete);
    connect(ui->actionEdit, &QAction::triggered, this, &MainWindow::onEdit);
    connect(ui->connectionView, &QTableView::doubleClicked, this, &MainWindow::onDoubleClicked);
    connect(ui->actionShare, &QAction::triggered, this, &MainWindow::onShare);
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::onConnect);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::onDisconnect);
    connect(ui->actionTest_Latency, &QAction::triggered, this, &MainWindow::onLatencyTest);
    connect(ui->actionView_Log, &QAction::triggered, this, &MainWindow::onViewLog);
    connect(ui->actionGeneral_Settings, &QAction::triggered, this, &MainWindow::onGeneralSettings);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);
    connect(ui->actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionReport_Bug, &QAction::triggered, this, &MainWindow::onReportBug);

    connect(ui->connectionView, &QTableView::activated, this, &MainWindow::checkCurrentIndex);

    checkCurrentIndex(ui->connectionView->currentIndex());
}

MainWindow::~MainWindow()
{
    systray->deleteLater();
    delete ui;
}

const QString MainWindow::aboutText = "<h3>Cross-Platform Shadowsocks GUI Client</h3><p>Version: " + QString(APP_VERSION) + "</p><p>Copyright © 2014-2015 Symeon Huang (<a href='https://twitter.com/librehat'>@librehat</a>)</p><p>Licensed under LGPLv3<br />Project Hosted at <a href='https://github.com/librehat/shadowsocks-qt5'>GitHub</a></p>";

const QUrl MainWindow::issueUrl = QUrl("https://github.com/librehat/shadowsocks-qt5/issues");

void MainWindow::minimizeToSysTray()
{
#ifdef UBUNTU_UNITY
    qApp->topLevelWindows().at(0)->hide();
    gtk_check_menu_item_set_active((GtkCheckMenuItem*)showItem, false);
#else
    this->hide();
#endif
}

bool MainWindow::isOnlyOneInstance() const
{
    return configHelper->isOnlyOneInstance();
}

#ifdef UBUNTU_UNITY
void onShow(GtkCheckMenuItem *menu, gpointer data)
{
    bool checked = gtk_check_menu_item_get_active(menu);
    QWindow *w = static_cast<QApplication *>(data)->topLevelWindows().at(0);
    if (checked) {
        w->show();
    }
    else {
        w->hide();
    }
}

void onQuit(GtkMenu *, gpointer data)
{
    static_cast<QApplication *>(data)->closeAllWindows();
}
#endif

void MainWindow::createSystemTray()
{
#ifdef UBUNTU_UNITY
        AppIndicator *indicator = app_indicator_new("Shadowsocks-Qt5", "shadowsocks-qt5", APP_INDICATOR_CATEGORY_OTHER);
        GtkWidget *menu = gtk_menu_new();

        showItem = gtk_check_menu_item_new_with_label(tr("Show").toLocal8Bit().constData());
        gtk_check_menu_item_set_active((GtkCheckMenuItem*)showItem, true);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), showItem);
        g_signal_connect(showItem, "toggled", G_CALLBACK(onShow), qApp);
        gtk_widget_show(showItem);

        GtkWidget *exitItem = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), exitItem);
        g_signal_connect(exitItem, "activate", G_CALLBACK(onQuit), qApp);
        gtk_widget_show(exitItem);

        app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
        app_indicator_set_menu(indicator, GTK_MENU(menu));
#else
        //desktop systray
        systrayMenu = new QMenu(this);
        systrayMenu->addAction(tr("Show"), this, SLOT(showWindow()));
        systrayMenu->addAction(QIcon::fromTheme("application-exit", QIcon::fromTheme("exit")), tr("Quit"), qApp, SLOT(quit()));

        systray->setIcon(QIcon(":/icons/icons/shadowsocks-qt5.png"));
        systray->setToolTip(QString("Shadowsocks-Qt5"));
        systray->setContextMenu(systrayMenu);
        connect(systray, &QSystemTrayIcon::activated, this, &MainWindow::onSystrayActivated);
        systray->show();
#endif
}

void MainWindow::showNotification(const QString &msg)
{
#ifdef Q_OS_LINUX
    //Using DBus to send message.
    QDBusMessage method = QDBusMessage::createMethodCall("org.freedesktop.Notifications","/org/freedesktop/Notifications", "org.freedesktop.Notifications", "Notify");
    QVariantList args;
    args << QCoreApplication::applicationName() << quint32(0) << "shadowsocks-qt5" << "Shadowsocks-Qt5" << msg << QStringList () << QVariantMap() << qint32(-1);
    method.setArguments(args);
    QDBusConnection::sessionBus().asyncCall(method);
#else
    if (systray) {
        systray->showMessage("Shadowsocks-Qt5", msg);
    }
#endif
}

void MainWindow::showWindow()
{
    this->show();
    this->setWindowState(Qt::WindowActive);
    this->activateWindow();
    ui->connectionView->setFocus();
}

void MainWindow::onAddManually()
{
    Connection *newCon = new Connection;
    newProfile(newCon);
}

void MainWindow::onAddScreenQRCode()
{
    QString uri;
    QList<QScreen *> screens = qApp->screens();
    for (QList<QScreen *>::iterator sc = screens.begin(); sc != screens.end(); ++sc) {
        QImage raw_sc = (*sc)->grabWindow(qApp->desktop()->winId()).toImage();
        QString result = URIHelper::decodeImage(raw_sc);
        if (!result.isNull()) {
            uri = result;
        }
    }
    if (uri.isNull()) {
        QMessageBox::critical(this, tr("QR Code Not Found"), tr("Can't find any QR code image that contains valid URI on your screen(s)."));
    } else {
        Connection *newCon = new Connection(uri, this);
        newProfile(newCon);
    }
}

void MainWindow::onAddQRCodeFile()
{
    QString qrFile = QFileDialog::getOpenFileName(this, tr("Open QR Code Image File"), QString(), "Images (*.png *jpg *jpeg *xpm)");
    if (!qrFile.isNull()) {
        QImage img(qrFile);
        QString uri = URIHelper::decodeImage(img);
        if (uri.isNull()) {
            QMessageBox::critical(this, tr("QR Code Not Found"), tr("Can't find any QR code image that contains valid URI on your screen(s)."));
        } else {
            Connection *newCon = new Connection(uri, this);
            newProfile(newCon);
        }
    }
}

void MainWindow::onAddFromURI()
{
    URIInputDialog *inputDlg = new URIInputDialog(this);
    connect(inputDlg, &URIInputDialog::acceptedURI, [&](const QString &uri){
        Connection *newCon = new Connection(uri, this);
        newProfile(newCon);
    });
    inputDlg->exec();
}

void MainWindow::onDelete()
{
    configHelper->deleteRow(ui->connectionView->currentIndex().row());
    checkCurrentIndex(ui->connectionView->currentIndex());
}

void MainWindow::onEdit()
{
    editRow(ui->connectionView->currentIndex().row());
}

void MainWindow::onDoubleClicked(const QModelIndex &index)
{
    editRow(index.row());
}

void MainWindow::onShare()
{
    QByteArray uri = configHelper->connectionAt(ui->connectionView->currentIndex().row())->getURI();
    ShareDialog *shareDlg = new ShareDialog(uri, this);
    shareDlg->exec();
}

void MainWindow::onConnect()
{
    int row = ui->connectionView->currentIndex().row();
    configHelper->connectionAt(row)->start();
    updateConnectionStatus(row);
    configHelper->updateLagAtRow(row);
    configHelper->updateTimeAtRow(row);
}

void MainWindow::onDisconnect()
{
    int row = ui->connectionView->currentIndex().row();
    configHelper->connectionAt(row)->stop();
    updateConnectionStatus(row);
}

void MainWindow::onLatencyTest()
{
    configHelper->latencyTestAtRow(ui->connectionView->currentIndex().row());
}

void MainWindow::onViewLog()
{
    Connection *con = configHelper->connectionAt(ui->connectionView->currentIndex().row());
    LogDialog *logDlg = new LogDialog(con->getLog(), this);
    connect(con, &Connection::newLogAvailable, logDlg, &LogDialog::append);
    logDlg->exec();
}

void MainWindow::onGeneralSettings()
{
    SettingsDialog *sDlg = new SettingsDialog(configHelper, this);
    sDlg->exec();
}

void MainWindow::updateConnectionStatus(int row)
{
    const bool &running = configHelper->connectionAt(row)->isRunning();
    ui->actionConnect->setEnabled(!running);
    ui->actionDisconnect->setEnabled(running);
}

void MainWindow::newProfile(Connection *newCon)
{
    EditDialog *editDlg = new EditDialog(newCon, this);
    if (editDlg->exec()) {//accepted
        configHelper->addConnection(newCon);
    } else {
        newCon->deleteLater();
    }
}

void MainWindow::editRow(int row)
{
    Connection *con = configHelper->connectionAt(row);
    EditDialog *editDlg = new EditDialog(con, this);
    if (editDlg->exec()) {
        configHelper->updateNameAtRow(row);
    }
}

void MainWindow::checkCurrentIndex(const QModelIndex &index)
{
    const bool valid = index.isValid();
    ui->actionConnect->setEnabled(valid);
    ui->actionDisconnect->setEnabled(valid);
    ui->actionTest_Latency->setEnabled(valid);
    ui->actionEdit->setEnabled(valid);
    ui->actionDelete->setEnabled(valid);
    ui->actionShare->setEnabled(valid);
    ui->actionView_Log->setEnabled(valid);

    if (valid) {
        updateConnectionStatus(index.row());
    }
}

void MainWindow::onSystrayActivated(QSystemTrayIcon::ActivationReason r)
{
    if (r != QSystemTrayIcon::Context) {
        showWindow();
    }
}

void MainWindow::onReportBug()
{
    QDesktopServices::openUrl(issueUrl);
}

void MainWindow::closeEvent(QCloseEvent *)
{
    minimizeToSysTray();
}

void MainWindow::onConnectionConnected(const QString &name)
{
    showNotification(name + " connected");
}

void MainWindow::onConnectionDisconnected(const QString &name)
{
    showNotification(name + " disconnected");
}
