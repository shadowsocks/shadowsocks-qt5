#include "statusnotifier.h"
#include "mainwindow.h"
#include <QApplication>
#ifdef Q_OS_UNIX
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusPendingCall>
#endif

StatusNotifier::StatusNotifier(MainWindow *w, bool startHiden, QObject *parent) :
    QObject(parent),
    window(w)
{
    systray.setIcon(QIcon(":/icons/icons/shadowsocks-qt5.png"));
    systray.setToolTip(QString("Shadowsocks-Qt5"));
    connect(&systray, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason r) {
        if (r != QSystemTrayIcon::Context) {
            this->activate();
        }
    });
    minimiseRestoreAction = new QAction(startHiden ? tr("Restore") : tr("Minimise"), this);
    connect(minimiseRestoreAction, &QAction::triggered, this, &StatusNotifier::activate);
    systrayMenu.addAction(minimiseRestoreAction);
    systrayMenu.addAction(QIcon::fromTheme("application-exit", QIcon::fromTheme("exit")), tr("Quit"), qApp, SLOT(quit()));
    systray.setContextMenu(&systrayMenu);

#ifdef Q_OS_UNIX
    QString de(getenv("XDG_CURRENT_DESKTOP"));
    useAppIndicator = appIndicatorDE.contains(de, Qt::CaseInsensitive);
    if (useAppIndicator) {
        createAppIndicator(startHiden);
    } else {
#endif
        systray.show();
#ifdef Q_OS_UNIX
    }
#endif
}

StatusNotifier::~StatusNotifier()
{
#ifdef Q_OS_WIN
    systray.hide();
#endif
}

const QStringList StatusNotifier::appIndicatorDE = QStringList() << "Unity" << "XFCE" << "Pantheon" << "LXDE";

#ifdef Q_OS_UNIX
void onAppIndicatorActivated(GtkMenuItem *, gpointer data)
{
    MainWindow *window = reinterpret_cast<MainWindow *>(data);
    if (!window->isVisible() || window->isMinimized()) {
        window->showNormal();
        window->activateWindow();
        window->raise();
    } else {
        window->hide();
    }
}

void onQuit(GtkMenu *, gpointer data)
{
    reinterpret_cast<QApplication *>(data)->quit();
}

void StatusNotifier::createAppIndicator(bool startHiden)
{
    AppIndicator *indicator = app_indicator_new("Shadowsocks-Qt5", "shadowsocks-qt5", APP_INDICATOR_CATEGORY_OTHER);
    GtkWidget *menu = gtk_menu_new();

    minimiseRestoreGtkItem = gtk_menu_item_new_with_label(
        tr(startHiden ? "Restore" : "Minimise").toLocal8Bit().constData());
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), minimiseRestoreGtkItem);
    g_signal_connect(minimiseRestoreGtkItem, "activate", G_CALLBACK(onAppIndicatorActivated), window);
    gtk_widget_show(minimiseRestoreGtkItem);

    GtkWidget *exitItem = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), exitItem);
    g_signal_connect(exitItem, "activate", G_CALLBACK(onQuit), qApp);
    gtk_widget_show(exitItem);

    app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_menu(indicator, GTK_MENU(menu));
}
#endif

bool StatusNotifier::isUsingAppIndicator() const
{
    return useAppIndicator;
}

void StatusNotifier::activate()
{
    if (!window->isVisible() || window->isMinimized()) {
        window->showNormal();
        window->activateWindow();
        window->raise();
    } else {
        window->hide();
    }
}

void StatusNotifier::showNotification(const QString &msg)
{
#ifdef Q_OS_UNIX
    //Using DBus to send message.
    QDBusMessage method = QDBusMessage::createMethodCall("org.freedesktop.Notifications","/org/freedesktop/Notifications", "org.freedesktop.Notifications", "Notify");
    QVariantList args;
    args << QCoreApplication::applicationName() << quint32(0) << "shadowsocks-qt5" << "Shadowsocks-Qt5" << msg << QStringList () << QVariantMap() << qint32(-1);
    method.setArguments(args);
    QDBusConnection::sessionBus().asyncCall(method);
#else
    systray.showMessage("Shadowsocks-Qt5", msg);
#endif
}

void StatusNotifier::onWindowVisibleChanged(bool visible)
{
#ifdef Q_OS_UNIX
    if (useAppIndicator) {
        gtk_menu_item_set_label(reinterpret_cast<GtkMenuItem *>(minimiseRestoreGtkItem), QObject::tr(visible ? "Minimise" : "Restore").toLocal8Bit().constData());
    } else {
#endif
        minimiseRestoreAction->setText(visible ? tr("Minimise") : tr("Restore"));
#ifdef Q_OS_UNIX
    }
#endif
}
