#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connection.h"

#include <QMenu>
#include <QCoreApplication>
#include <QDesktopWidget>
#include <QWindow>
#include <QDir>

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
#ifdef Q_OS_WIN
    jsonconfigFile = QCoreApplication::applicationDirPath() + "/config.json";
#else
    QDir ssConfigDir = QDir::homePath() + "/.config/shadowsocks-qt5";
    jsonconfigFile = ssConfigDir.absolutePath() + "/config.json";
    if (!ssConfigDir.exists()) {
        ssConfigDir.mkpath(ssConfigDir.absolutePath());
    }
#endif

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
}

MainWindow::~MainWindow()
{
    systray->deleteLater();
    delete ui;
}

const QString MainWindow::aboutText = "<h3>Cross-Platform Shadowsocks GUI Client</h3><p>Version: " + QString(APP_VERSION) + "</p><p>Copyright © 2014-2015 Symeon Huang (<a href='https://twitter.com/librehat'>@librehat</a>)</p><p>Licensed under LGPLv3<br />Project Hosted at <a href='https://github.com/librehat/shadowsocks-qt5'>GitHub</a></p>";

void MainWindow::minimizeToSysTray()
{
#ifdef UBUNTU_UNITY
    qApp->topLevelWindows().at(0)->hide();
    gtk_check_menu_item_set_active((GtkCheckMenuItem*)showItem, false);
#else
    this->hide();
#endif
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
        systrayMenu->addAction(QIcon::fromTheme("application-exit", QIcon::fromTheme("exit")), tr("Quit"), qApp, SLOT(exit()));

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

void MainWindow::onSystrayActivated(QSystemTrayIcon::ActivationReason r)
{
    if (r != QSystemTrayIcon::Context) {
        showWindow();
    }
}

void MainWindow::closeEvent(QCloseEvent *)
{
    minimizeToSysTray();
}
