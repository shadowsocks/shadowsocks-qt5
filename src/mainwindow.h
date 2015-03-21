/*
 * MainWindow class
 * Lots of variables, functions are included in this class
 * Copyright 2014-2015 Symeon Huang <hzwhuang@gmail.com>
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCoreApplication>
#include <QDesktopWidget>
#include <QAbstractButton>
#include <QPushButton>
#include <QSystemTrayIcon>
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
#include "addprofiledialog.h"

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
    void onAddButtonClicked(bool);
    void onBackendTypeChanged(const QString &);
    void onDeleteButtonClicked();
    void onLAddrEditFinished(const QString &);
    void onLPortEditFinished(const QString &);
    void onMethodChanged(const QString &);
    void onAddProfileDialogAccepted(const QString &, bool, const QString &);
    void onAddProfileDialogRejected(const bool);
    void onBackendToolButtonPressed();
    void onConfigurationChanged(bool);
    void onCurrentProfileChanged(int);
    void onCustomArgsEditFinished(const QString &);
    void onShareButtonClicked();
    void onProcessReadyRead(const QByteArray &);
    void onProcessStarted();
    void onProcessStopped();
    void onProfileResetClicked();
    void onProfileSaveClicked();
    void onPasswordEditFinished(const QString &);
    void onServerEditFinished(const QString &);
    void showWindow();
    void onSPortEditFinished(const QString &);
    void onSystrayActivated(QSystemTrayIcon::ActivationReason);
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
    void onUseSystrayToggled(bool);
    void onSingleInstanceToggled(bool);
    void saveConfig();

private:
    AddProfileDialog *addProfileDlg;
    bool verboseOutput;
    IP4Validator ipv4addrValidator;
    PortValidator portValidator;
    QString jsonconfigFile;
    QMenu *systrayMenu;
    QSystemTrayIcon *systray;
    SS_Process *ssProcess;
    SSProfile *current_profile;
    static const QString aboutText;
    Ui::MainWindow *ui;
#ifdef UBUNTU_UNITY
    GtkWidget *showItem;
#endif
    void createSystemTray();
    void showNotification(const QString &);
    void blockChildrenSignals(bool);

protected:
    void changeEvent(QEvent *);
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
