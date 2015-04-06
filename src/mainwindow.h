/*
 * MainWindow class
 * Lots of variables, functions are included in this class
 * Copyright 2014-2015 Symeon Huang <hzwhuang@gmail.com>
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QCloseEvent>
#include "confighelper.h"

#ifdef UBUNTU_UNITY
#undef signals
extern "C"
{
#include <libappindicator/app-indicator.h>
#include <gtk/gtk.h>
}
#define signals public
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public:
    void minimizeToSysTray();

private slots:
    void onAddManually();
    void onAddScreenQRCode();
    void onAddQRCodeFile();
    void onAddFromURI();
    void onDelete();
    void onEdit();
    void onDoubleClicked(const QModelIndex &index);
    void onShare();
    void onViewLog();
    void onGeneralSettings();
    void checkCurrentIndex(const QModelIndex &index);
    inline void onAbout() { QMessageBox::about(this, tr("About"), aboutText); }
    void showWindow();
    void onSystrayActivated(QSystemTrayIcon::ActivationReason);
    void onReportBug();

private:
    ConfigHelper *configHelper;
    QMenu *systrayMenu;
    QSystemTrayIcon *systray;
    Ui::MainWindow *ui;
#ifdef UBUNTU_UNITY
    GtkWidget *showItem;
#endif

    void newProfile(Connection *);
    void editRow(int row);
    void createSystemTray();
    void showNotification(const QString &);
    void blockChildrenSignals(bool);

    static const QStringList headerLabels;
    static const QString aboutText;
    static const QUrl issueUrl;

protected:
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
