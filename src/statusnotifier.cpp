#include "statusnotifier.h"

#include <QApplication>
#include <QWindow>
#ifdef Q_OS_UNIX
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusPendingCall>
#endif
#include <stdlib.h>

#ifdef Q_OS_UNIX
void onAppIndicatorActivated(GtkMenuItem *item, gpointer data)
{
    MainWindow *w = reinterpret_cast<MainWindow *>(data);
    if (w->isVisible()) {
        w->hide();
        gtk_menu_item_set_label(item, QObject::tr("Restore").toLocal8Bit().constData());
    } else {
        w->show();
        gtk_menu_item_set_label(item, QObject::tr("Minimise").toLocal8Bit().constData());
    }
}

void onQuit(GtkMenu *, gpointer data)
{
    static_cast<QApplication *>(data)->quit();
}
#endif

StatusNotifier::StatusNotifier(MainWindow &w, QObject *parent) :
    QObject(parent),
    systrayMenu(nullptr),
    minimiseRestoreAction(nullptr),
    window(w)
{
    systray = new QSystemTrayIcon(this);
    systray->setIcon(QIcon(":/icons/icons/shadowsocks-qt5.png"));
    systray->setToolTip(QString("Shadowsocks-Qt5"));
    connect(systray, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason r) {
        if (r != QSystemTrayIcon::Context) {
            this->activate();
        }
    });

#ifdef Q_OS_UNIX
    QString de(getenv("XDG_CURRENT_DESKTOP"));
    useAppIndicator = appIndicatorDE.contains(de, Qt::CaseInsensitive);
    if (useAppIndicator) {
        createAppIndicator();
    } else {
#endif
        createSystemTray();
#ifdef Q_OS_UNIX
    }
#endif
}

StatusNotifier::~StatusNotifier()
{
#ifdef Q_OS_WIN
    systray->hide();
#endif
    if (systrayMenu) {
        systrayMenu->deleteLater();
    }
}

const QStringList StatusNotifier::appIndicatorDE = QStringList() << "Unity" << "XFCE" << "Pantheon" << "GNOME" << "LXDE";

bool StatusNotifier::isUsingAppIndicator() const
{
    return useAppIndicator;
}

#ifdef Q_OS_UNIX
void StatusNotifier::createAppIndicator()
{
    AppIndicator *indicator = app_indicator_new("Shadowsocks-Qt5", "shadowsocks-qt5", APP_INDICATOR_CATEGORY_OTHER);
    GtkWidget *menu = gtk_menu_new();

    minimiseRestoreGtkItem = gtk_menu_item_new_with_label(tr("Minimise").toLocal8Bit().constData());
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), minimiseRestoreGtkItem);
    g_signal_connect(minimiseRestoreGtkItem, "activate", G_CALLBACK(onAppIndicatorActivated), &window);
    gtk_widget_show(minimiseRestoreGtkItem);

    GtkWidget *exitItem = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), exitItem);
    g_signal_connect(exitItem, "activate", G_CALLBACK(onQuit), &window);
    gtk_widget_show(exitItem);

    app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_menu(indicator, GTK_MENU(menu));
}
#endif

void StatusNotifier::createSystemTray()
{
    systrayMenu = new QMenu;
    minimiseRestoreAction = new QAction(tr("Minimise"), this);
    connect(minimiseRestoreAction, &QAction::triggered, this, &StatusNotifier::activate);
    systrayMenu->addAction(minimiseRestoreAction);
    systrayMenu->addAction(QIcon::fromTheme("application-exit", QIcon::fromTheme("exit")), tr("Quit"), qApp, SLOT(quit()));

    systray->setContextMenu(systrayMenu);
    systray->show();
}

void StatusNotifier::hideTopWindow()
{
    window.hide();
}

void StatusNotifier::showTopWindow()
{
    window.show();
}

void StatusNotifier::activate()
{
    if (window.isVisible()) {
        window.hide();
    } else {
        window.show();
        window.setFocus();
    }
    if (minimiseRestoreAction) {
        minimiseRestoreAction->setText(window.isVisible() ? tr("Minimise") : tr("Restore"));
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
    systray->showMessage("Shadowsocks-Qt5", msg);
#endif
}
