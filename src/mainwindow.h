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
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Configuration *m_conf;

signals:
    void configurationChanged(bool saved = false);

public slots:
    void startButtonPressed();

private slots:
    void onCurrentProfileChanged(int);
    void onBackendToolButtonPressed();
    void backendTypeChanged(const QString &);
    void addProfileDialogue(bool);
    void onAddProfileDialogueAccepted(const QString &, bool, const QString &);
    void onAddProfileDialogueRejected(bool enforce = false);
    void profileEditButtonClicked(QAbstractButton*);
    void stopButtonPressed();
    void deleteProfile();
    void processStarted();
    void processStopped();
    void systrayActivated(QSystemTrayIcon::ActivationReason);
    void showorhideWindow();
    void onReadReadyProcess(const QByteArray &o);
    void onConfigurationChanged(bool);
    void serverEditFinished(const QString &);
    void sportEditFinished(const QString &);
    void pwdEditFinished(const QString &);
    void laddrEditFinished(const QString &);
    void lportEditFinished(const QString &);
    void methodChanged(const QString &);
    void timeoutChanged(int);
#ifdef __linux__
    void tcpFastOpenChanged(bool);
#endif
    void autoHideToggled(bool);
    void autoStartToggled(bool);
    void debugToggled(bool);
    void transculentToggled(bool);
    void aboutButtonClicked();
    void saveConfig();

private:
    Ui::MainWindow *ui;
    SS_Process ss_local;
    SSProfile *current_profile;
    QString jsonconfigFile;
    IP4Validator ipv4addrValidator;
    PortValidator portValidator;
    QSystemTrayIcon systray;
    QMenu systrayMenu;
    AddProfileDialogue addProfileDlg;
    void updateTranslucent(bool translucent);
    static const QString aboutText;

protected:
    void changeEvent(QEvent *);
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
