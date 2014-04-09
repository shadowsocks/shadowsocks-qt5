/*
 * MainWindow class
 * Lots of variables, functions are included in this class
 * Copyright 2014 William Wong <librehat@outlook.com>
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QAbstractButton>
#include <QPushButton>
#include <QDebug>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QFileDialog>
#include <QFile>
#include <QStandardPaths>
#include <QInputDialog>
#include "profiles.h"
#include "ss_process.h"

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

private slots:
    void oncurrentProfileChanged(int);
    void getSSLocalPath();
    void addProfileDialogue(bool enforce);
    void profileEditButtonClicked(QAbstractButton *b);
    void apply_abort_Accepted();
    void apply_abort_Rejected();
    void deleteProfile();
    void processStarted();
    void processStopped();
    void systrayActivated(QSystemTrayIcon::ActivationReason);
    void showorhideWindow();
    void onreadReadyProcess(const QByteArray &o);

private:
    Ui::MainWindow *ui;
    QString detectSSLocal();
    QString jsonconfigFile;
    Profiles *m_profile;
    QSystemTrayIcon systray;
    QMenu systrayMenu;

protected:
    void changeEvent(QEvent *);
};

#endif // MAINWINDOW_H
