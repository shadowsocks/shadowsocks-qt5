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
    void startButtonPressed();

private slots:
    inline void stopButtonPressed() { ss_local.stop(); }
    void addProfileDialogue(bool);
    void backendTypeChanged(const QString &);
    void deleteProfile();
    void laddrEditFinished(const QString &);
    void lportEditFinished(const QString &);
    void methodChanged(const QString &);
    void onAddProfileDialogueAccepted(const QString &, bool, const QString &);
    void onAddProfileDialogueRejected(bool enforce = false);
    void onBackendToolButtonPressed();
    void onConfigurationChanged(bool);
    void onCurrentProfileChanged(int);
    void onCustomArgsEditFinished(const QString &);
    void onReadReadyProcess(const QByteArray &o);
    void processStarted();
    void processStopped();
    void profileEditButtonClicked(QAbstractButton*);
    void pwdEditFinished(const QString &);
    void serverEditFinished(const QString &);
    void showWindow();
    void sportEditFinished(const QString &);
    void systrayActivated(QSystemTrayIcon::ActivationReason);
    void timeoutChanged(int);
#ifdef Q_OS_LINUX
    void tcpFastOpenChanged(bool);
#endif
    inline void aboutButtonClicked() { QMessageBox::about(this, tr("About"), aboutText); }
    void autoHideToggled(bool);
    void autoStartToggled(bool);
    void debugToggled(bool);
    void relativePathToggled(bool);
    void saveConfig();
    void transculentToggled(bool);

private:
    AddProfileDialogue *addProfileDlg;
    bool verboseOutput;
    IP4Validator ipv4addrValidator;
    PortValidator portValidator;
    QMenu systrayMenu;
    QString jsonconfigFile;
    QSystemTrayIcon systray;
    SS_Process ss_local;
    SSProfile *current_profile;
    static const QString aboutText;
    Ui::MainWindow *ui;
    void showNotification(const QString &);
    void updateTranslucent(bool translucent);

#ifdef Q_OS_LINUX
    bool isUbuntuUnity;
#endif

protected:
    void changeEvent(QEvent *);
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
