/*
 * StatusNotifier Class
 * Copyright 2015 Symeon Huang <hzwhuang@gmail.com>
 */

#ifndef STATUSNOTIFIER_H
#define STATUSNOTIFIER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QStringList>

#ifdef UBUNTU_UNITY
#undef signals
extern "C"
{
#include <libappindicator/app-indicator.h>
#include <gtk/gtk.h>
}
#define signals public
#endif

class StatusNotifier : public QObject
{
    Q_OBJECT
public:
    explicit StatusNotifier(QObject *parent = 0);
    ~StatusNotifier();

    bool isUsingAppIndicator() const;

signals:
    void activated();

public slots:
    void onMainWindowVisibilityChanged(const bool visible);
    void showNotification(const QString &);

private:
#ifdef UBUNTU_UNITY
    GtkWidget *showItem;
#endif
    QMenu *systrayMenu;
    QAction *minimiseRestoreAction;
    QSystemTrayIcon *systray;

    bool useAppIndicator;

    void createSystemTray();
    void createAppIndicator();

    //desktop environments that need application indicator
    static const QStringList appIndicatorDE;
};

#endif // STATUSNOTIFIER_H
