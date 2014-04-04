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
    QProcess sslocal;

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
    void autoStartSSLocal();
    void processStarted();
    void processStopped();
    void systrayActivated(QSystemTrayIcon::ActivationReason);
    void showorhideWindow();

private:
    Ui::MainWindow *ui;
    LogDialogue logdlg;
    QString detectSSLocal();
    QString jsonconfigFile;
    SSProfile current_profile;
    Profiles *m_profile;
    SS_Process ss_local;
    QSystemTrayIcon systray;
    QMenu systrayMenu;
};

#endif // MAINWINDOW_H
