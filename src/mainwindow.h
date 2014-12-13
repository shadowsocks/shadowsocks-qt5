/*
 * MainWindow class
 * Lots of variables, functions are included in this class
 * Copyright 2014 William Wong <librehat@outlook.com>
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCoreApplication>
#include <QDesktopWidget>
#include <QAbstractButton>
#include <QPushButton>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QFileDialog>
#include <QStandardPaths>
#include <QInputDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include "ssprofile.h"
#include "configuration.h"
#include "ss_process.h"
#include "ssvalidator.h"
#include "ip4validator.h"
#include "portvalidator.h"
#include "addprofiledialogue.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(bool verbose = false, QWidget *parent = 0);
    ~MainWindow();
    Configuration *m_conf;

signals:
    void configurationChanged(bool saved = false);

public:
    void minimizeToSysTray();

public slots:
    void onStartButtonPressed();

private slots:
    inline void onStopButtonPressed() { ssProcess->stop(); }
    void addProfileDialogue(bool);
    void onBackendTypeChanged(const QString &);
    void deleteProfile();
    void onLAddrEditFinished(const QString &);
    void onLPortEditFinished(const QString &);
    void onMethodChanged(const QString &);
    void onAddProfileDialogueAccepted(const QString &, bool, const QString &);
    void onAddProfileDialogueRejected(bool enforce = false);
    void onBackendToolButtonPressed();
    void onConfigurationChanged(bool);
    void onCurrentProfileChanged(int);
    void onCustomArgsEditFinished(const QString &);
    void onShareButtonClicked();
    void onProcessReadyRead(const QByteArray &);
    void onProcessStarted();
    void onProcessStopped();
    void onProfileEditButtonClicked(QAbstractButton*);
    void onPasswordEditFinished(const QString &);
    void onServerEditFinished(const QString &);
    void showWindow();
    void onSPortEditFinished(const QString &);
    void systrayActivated(QSystemTrayIcon::ActivationReason);
    void onTimeoutChanged(int);
#ifdef Q_OS_LINUX
    void onTcpFastOpenChanged(bool);
#endif
    inline void onAboutButtonClicked() { QMessageBox::about(this, tr("About"), aboutText); }
    void onAutoHideToggled(bool);
    void onAutoStartToggled(bool);
    void onDebugToggled(bool);
    void onRelativePathToggled(bool);
    void onTransculentToggled(bool);
    void saveConfig();

private:
    AddProfileDialogue *addProfileDlg;
    bool verboseOutput;
    IP4Validator ipv4addrValidator;
    PortValidator portValidator;
    QMenu systrayMenu;
    QString jsonconfigFile;
    QSystemTrayIcon systray;
    SS_Process *ssProcess;
    SSProfile *current_profile;
    static const QString aboutText;
    Ui::MainWindow *ui;
    void showNotification(const QString &);
    void blockChildrenSignals(bool);

#ifdef Q_OS_LINUX
    bool isUbuntuUnity;
#endif

protected:
    void changeEvent(QEvent *);
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
