/*
 * MainWindow class
 * Lots of variables, functions are included in this class
 * Copyright 2014 William Wong <librehat@outlook.com>
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include "profiles.h"
#include "ss_process.h"
#include "logdialogue.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    SS_Process ss_local;
    SSProfile current_profile;

signals:
    void currentProfileChanged(int);

public slots:
    void oncurrentProfileChanged(int);
    void getSSLocalPath();
    void addProfileDialogue(bool enforce);
    void startbuttonPressed();
    void savebuttonPressed();
    void showLogDialogue();
    void deleteProfile();
    void revertbuttonPressed();
    void processStarted();
    void processStopped();
    void systrayActivated(QSystemTrayIcon::ActivationReason);
    void showorhideWindow();

private:
    Ui::MainWindow *ui;
    LogDialogue logdlg;
    QString detectSSLocal();
    QString jsonconfigFile;
    Profiles *m_profile;
    QSystemTrayIcon systray;
    QMenu systrayMenu;

protected:
    void changeEvent(QEvent *);
};

#endif // MAINWINDOW_H
