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
#include <QSystemTrayIcon>
#include <QMenu>
#include <QFileDialog>
#include <QFile>
#include <QStandardPaths>
#include <QInputDialog>
#include <QMessageBox>
#include "ssprofile.h"
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
    Profiles *m_profile;

signals:
    //void currentProfileChanged(int);
    void configurationChanged(bool saved = false);
    void miscConfigurationChanged(bool saved = false);

public slots:
    void startButtonPressed();

private slots:
    void onProfileComboBoxActivated(int);
    void onCurrentProfileChanged(int);
    void onBackendToolButtonPressed();
    void backendTypeChanged(const QString &);
    void addProfileDialogue(bool);
    void profileEditButtonClicked(QAbstractButton*);
    void stopButtonPressed();
    void deleteProfile();
    void processStarted();
    void processStopped();
    void systrayActivated(QSystemTrayIcon::ActivationReason);
    void showorhideWindow();
    void onReadReadyProcess(const QByteArray &o);
    void onConfigurationChanged(bool);
    void onMiscConfigurationChanged(bool);
    void serverEditFinished(const QString &);
    void sportEditFinished(const QString &);
    void pwdEditFinished(const QString &);
    void laddrEditFinished(const QString &);
    void lportEditFinished(const QString &);
    void methodChanged(const QString &);
    void timeoutChanged(int);
    void autoHideChecked(int);
    void autoStartChecked(int);
    void debugChecked(int);
    void miscButtonBoxClicked();
    void aboutButtonClicked();

private:
    Ui::MainWindow *ui;
    SS_Process ss_local;
    SSProfile current_profile;
    QString detectSSLocal();
    QString jsonconfigFile;
    QSystemTrayIcon systray;
    QMenu systrayMenu;
    void saveProfile();
    void saveMiscConfig();
    void checkIfSaved();

protected:
    void changeEvent(QEvent *);
};

#endif // MAINWINDOW_H
