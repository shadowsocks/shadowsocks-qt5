#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "connection.h"
#include "editdialog.h"
#include "urihelper.h"
#include "uriinputdialog.h"
#include "sharedialog.h"
#include "logdialog.h"
#include "statusdialog.h"
#include "settingsdialog.h"

#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QScreen>
#include <QWindow>
#include <stdlib.h>
#include <botan/version.h>

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
    QString de(getenv("XDG_CURRENT_DESKTOP"));
    useAppIndicator = appIndicatorDE.contains(de, Qt::CaseInsensitive);

    configHelper = new ConfigHelper(this);
    ui->connectionView->setModel(configHelper->getModel());
    ui->connectionView->resizeColumnsToContents();
    connect(configHelper, &ConfigHelper::rowStatusChanged, this, &MainWindow::onConnectionStatusChanged);
    connect(configHelper, &ConfigHelper::connectionStartFailed, [this] {
        QMessageBox::critical(this, tr("Connect Failed"), tr("Local address or port may be invalid or already in use."));
    });
    connect(configHelper, &ConfigHelper::message, this, &MainWindow::showNotification);
    connect(ui->actionSaveManually, &QAction::triggered, configHelper, &ConfigHelper::save);
    connect(ui->actionTest_All_Latency, &QAction::triggered, configHelper, &ConfigHelper::testAllLags);

    /*
     * There is a bug on KDE Frameworks 5: https://bugs.kde.org/show_bug.cgi?id=343976
     * As a simple work around, we give up our ownership and use deleteLater() function
     * in MainWindow's destructor.
     *
     */
    systray = new QSystemTrayIcon;
    if (isUsingAppIndicator()) {
        createAppIndicator();
    } else {
        createSystemTray();
    }

    //Move to the center of the screen
    this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());

    connect(ui->actionImportGUIJson, &QAction::triggered, this, &MainWindow::onImportGuiJson);
    connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionManually, &QAction::triggered, this, &MainWindow::onAddManually);
    connect(ui->actionQR_Code, &QAction::triggered, this, &MainWindow::onAddScreenQRCode);
    connect(ui->actionQR_Code_from_File, &QAction::triggered, this, &MainWindow::onAddQRCodeFile);
    connect(ui->actionURI, &QAction::triggered, this, &MainWindow::onAddFromURI);
    connect(ui->actionFrom_config_json, &QAction::triggered, this, &MainWindow::onAddFromConfigJSON);
    connect(ui->actionDelete, &QAction::triggered, this, &MainWindow::onDelete);
    connect(ui->actionEdit, &QAction::triggered, this, &MainWindow::onEdit);
    connect(ui->actionShare, &QAction::triggered, this, &MainWindow::onShare);
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::onConnect);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::onDisconnect);
    connect(ui->actionTest_Latency, &QAction::triggered, this, &MainWindow::onLatencyTest);
    connect(ui->actionView_Log, &QAction::triggered, this, &MainWindow::onViewLog);
    connect(ui->actionStatus, &QAction::triggered, this, &MainWindow::onStatus);
    connect(ui->actionMoveUp, &QAction::triggered, this, &MainWindow::onMoveUp);
    connect(ui->actionMoveDown, &QAction::triggered, this, &MainWindow::onMoveDown);
    connect(ui->actionGeneral_Settings, &QAction::triggered, this, &MainWindow::onGeneralSettings);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);
    connect(ui->actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionReport_Bug, &QAction::triggered, this, &MainWindow::onReportBug);

    connect(ui->connectionView, &QTableView::clicked, this, &MainWindow::checkCurrentIndex);
    connect(ui->connectionView, &QTableView::activated, this, &MainWindow::checkCurrentIndex);
    connect(ui->connectionView, &QTableView::doubleClicked, this, &MainWindow::onDoubleClicked);

    /* set custom context menu */
    ui->connectionView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->connectionView, &QTableView::customContextMenuRequested, this, &MainWindow::onCustomContextMenuRequested);

    checkCurrentIndex(ui->connectionView->currentIndex());

    configHelper->startAllAutoStart();//do this at last so that all signal-slot should've been connected
}

MainWindow::~MainWindow()
{
#ifdef Q_OS_WIN
    systray->hide();
#endif
    systray->deleteLater();
    delete ui;
}

const QStringList MainWindow::appIndicatorDE = QStringList() << "Unity" << "XFCE" << "Pantheon";

const QUrl MainWindow::issueUrl = QUrl("https://github.com/librehat/shadowsocks-qt5/issues");

void MainWindow::minimizeToSysTray()
{
#ifdef UBUNTU_UNITY
    if (isUsingAppIndicator()) {
        qApp->topLevelWindows().at(0)->hide();
        gtk_check_menu_item_set_active((GtkCheckMenuItem*)showItem, false);
    } else {
        this->hide();
    }
#else
    this->hide();
#endif
}

void MainWindow::onShowSignalRecv()
{
#ifdef UBUNTU_UNITY
    if (isUsingAppIndicator()) {
        qApp->topLevelWindows().at(0)->show();
        gtk_check_menu_item_set_active((GtkCheckMenuItem*)showItem, true);
    } else {
        this->showWindow();
    }
#else
    this->showWindow();
#endif
}

bool MainWindow::isOnlyOneInstance() const
{
    return configHelper->isOnlyOneInstance();
}

bool MainWindow::isHideWindowOnStartup() const
{
    return configHelper->isHideWindowOnStartup();
}

#ifdef UBUNTU_UNITY
void onShow(GtkCheckMenuItem *menu, gpointer data)
{
    bool checked = gtk_check_menu_item_get_active(menu);
    QWindow *w = static_cast<QApplication *>(data)->topLevelWindows().at(0);
    if (checked) {
        w->show();
    } else {
        w->hide();
    }
}

void onQuit(GtkMenu *, gpointer data)
{
    static_cast<QApplication *>(data)->quit();
}
#endif

bool MainWindow::isUsingAppIndicator() const
{
#ifdef UBUNTU_UNITY
    return useAppIndicator;
#else
    return false;
#endif
}

void MainWindow::createAppIndicator()
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
    QMessageBox::critical(this, tr("Error"), tr("The application wasn't built with aplication indicator support."));
#endif
}

void MainWindow::createSystemTray()
{
    systrayMenu = new QMenu(this);
    systrayMenu->addAction(tr("Show"), this, SLOT(showWindow()));
    systrayMenu->addAction(tr("Hide"), this, SLOT(minimizeToSysTray()));
    systrayMenu->addAction(QIcon::fromTheme("application-exit", QIcon::fromTheme("exit")), tr("Quit"), qApp, SLOT(quit()));

    systray->setIcon(QIcon(":/icons/icons/shadowsocks-qt5.png"));
    systray->setToolTip(QString("Shadowsocks-Qt5"));
    systray->setContextMenu(systrayMenu);
    connect(systray, &QSystemTrayIcon::activated, this, &MainWindow::onSystrayActivated);
    systray->show();
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

void MainWindow::onImportGuiJson()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Import Connections from gui-config.json"), QString(), "GUI Configuration (gui-config.json)");
    if (!file.isNull()) {
        configHelper->importGuiConfigJson(file);
    }
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
    connect(inputDlg, &URIInputDialog::finished, inputDlg, &URIInputDialog::deleteLater);
    connect(inputDlg, &URIInputDialog::acceptedURI, [&](const QString &uri){
        Connection *newCon = new Connection(uri, this);
        newProfile(newCon);
    });
    inputDlg->exec();
}

void MainWindow::onAddFromConfigJSON()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open config.json"), QString(), "JSON (*.json)");
    if (!file.isNull()) {
        Connection *con = configHelper->configJsonToConnection(file);
        if (con) {
            newProfile(con);
        }
    }
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
    int row = index.row();
    if (configHelper->connectionAt(row)->isRunning()) {
        onStatus();
    } else {
        editRow(row);
    }
}

void MainWindow::onShare()
{
    QByteArray uri = configHelper->connectionAt(ui->connectionView->currentIndex().row())->getURI();
    ShareDialog *shareDlg = new ShareDialog(uri, this);
    connect(shareDlg, &ShareDialog::finished, shareDlg, &ShareDialog::deleteLater);
    shareDlg->exec();
}

void MainWindow::onConnect()
{
    int row = ui->connectionView->currentIndex().row();
    Connection *con = configHelper->connectionAt(row);
    if (con->isValid()) {
        con->start();
        configHelper->updateTimeAtRow(row);
    } else {
        QMessageBox::critical(this, tr("Invalid"), tr("The connection's profile is invalid!"));
    }
}

void MainWindow::onDisconnect()
{
    int row = ui->connectionView->currentIndex().row();
    configHelper->connectionAt(row)->stop();
}

void MainWindow::onConnectionStatusChanged(const int row, const bool running)
{
    if (ui->connectionView->currentIndex().row() == row) {
        ui->actionConnect->setEnabled(!running);
        ui->actionDisconnect->setEnabled(running);
    }
}

void MainWindow::onLatencyTest()
{
    configHelper->latencyTestAtRow(ui->connectionView->currentIndex().row());
}

void MainWindow::onViewLog()
{
    Connection *con = configHelper->connectionAt(ui->connectionView->currentIndex().row());
    LogDialog *logDlg = new LogDialog(con, this);
    connect(logDlg, &LogDialog::finished, logDlg, &LogDialog::deleteLater);
    logDlg->exec();
}

void MainWindow::onStatus()
{
    Connection *con = configHelper->connectionAt(ui->connectionView->currentIndex().row());
    StatusDialog *statusDlg = new StatusDialog(con, this);
    connect(statusDlg, &StatusDialog::finished, statusDlg, &StatusDialog::deleteLater);
    statusDlg->exec();
}

void MainWindow::onMoveUp()
{
    QModelIndex index = configHelper->moveUp(ui->connectionView->currentIndex().row());
    ui->connectionView->setCurrentIndex(index);
    checkCurrentIndex(index);
}

void MainWindow::onMoveDown()
{
    QModelIndex index = configHelper->moveDown(ui->connectionView->currentIndex().row());
    ui->connectionView->setCurrentIndex(index);
    checkCurrentIndex(index);
}

void MainWindow::onGeneralSettings()
{
    SettingsDialog *sDlg = new SettingsDialog(configHelper, this);
    connect(sDlg, &SettingsDialog::finished, sDlg, &SettingsDialog::deleteLater);
    sDlg->exec();
}

void MainWindow::newProfile(Connection *newCon)
{
    EditDialog *editDlg = new EditDialog(newCon, this);
    connect(editDlg, &EditDialog::finished, editDlg, &EditDialog::deleteLater);
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
    connect(editDlg, &EditDialog::finished, editDlg, &EditDialog::deleteLater);
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
    ui->actionStatus->setEnabled(valid);

    if (valid) {
        const bool &running = configHelper->connectionAt(index.row())->isRunning();
        ui->actionConnect->setEnabled(!running);
        ui->actionDisconnect->setEnabled(running);

        ui->actionMoveUp->setEnabled(index.row() > 0);
        ui->actionMoveDown->setEnabled(index.row() < configHelper->size() - 1);
    }
}

void MainWindow::onSystrayActivated(QSystemTrayIcon::ActivationReason r)
{
    if (r != QSystemTrayIcon::Context) {
        if (this->isVisible()) {
            minimizeToSysTray();
        } else {
            showWindow();
        }
    }
}

void MainWindow::onAbout()
{
    QString text = QString("<h1>Shadowsocks-Qt5</h1><p><b>Version %1</b><br />Using libQtShadowsocks %2<br />Using Botan %3.%4.%5</p><p>Copyright © 2014-2015 Symeon Huang (<a href='https://twitter.com/librehat'>@librehat</a>)</p><p>License: <a href='http://www.gnu.org/licenses/lgpl.html'>GNU Lesser General Public License Version 3</a><br />Project Hosted at <a href='https://github.com/librehat/shadowsocks-qt5'>GitHub</a></p>").arg(QStringLiteral(APP_VERSION)).arg(QSS::Common::version().data()).arg(Botan::version_major()).arg(Botan::version_minor()).arg(Botan::version_patch());
    QMessageBox::about(this, tr("About"), text);
}

void MainWindow::onReportBug()
{
    QDesktopServices::openUrl(issueUrl);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    e->ignore();
    minimizeToSysTray();
}

void MainWindow::onCustomContextMenuRequested(const QPoint &pos)
{
    this->checkCurrentIndex(ui->connectionView->indexAt(pos));
    ui->menuConnection->popup(ui->connectionView->viewport()->mapToGlobal(pos));
}
