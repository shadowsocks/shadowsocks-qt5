#include <QDesktopWidget>
#include <QFileDialog>
#include <QFile>
#include <QStandardPaths>
#include <QInputDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //SIGNALs and SLOTs
    connect(ui->sslocalToolButton, &QToolButton::clicked, this, &MainWindow::getSSLocalPath);
    connect(ui->serverComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(oncurrentProfileChanged(int)));
    connect(ui->addProfileButton, &QToolButton::clicked, this, &MainWindow::addProfileDialogue);
    connect(ui->saveButton, &QToolButton::clicked, this, &MainWindow::savebuttonPressed);
    connect(ui->startButton, &QToolButton::clicked, this, &MainWindow::startbuttonPressed);
    connect(ui->revertButton, &QToolButton::clicked, this, &MainWindow::revertbuttonPressed);
    connect(ui->logButton, &QToolButton::clicked, this, &MainWindow::showLogDialogue);
    connect(ui->delProfileButton, &QToolButton::clicked, this, &MainWindow::deleteProfile);
    connect(this, &MainWindow::currentProfileChanged, this, &MainWindow::oncurrentProfileChanged);
    connect(&ss_local, &SS_Process::readReadyProcess, &logdlg, &LogDialogue::onreadReadyProcess);
    connect(&ss_local, &SS_Process::sigstart, this, &MainWindow::processStarted);
    connect(&ss_local, &SS_Process::sigstop, this, &MainWindow::processStopped);
    connect(&systray, &QSystemTrayIcon::activated, this, &MainWindow::systrayActivated);

    //initialisation
#ifdef _WIN32
    jsonconfigFile = QCoreApplication::applicationDirPath() + "/gui-config.json";
#else
    jsonconfigFile = QDir::homePath() + "/.config/shadowsocks/gui-config.json";
#endif
    m_profile = new Profiles(jsonconfigFile);
    ui->sslocalEdit->setText(detectSSLocal());
    ui->serverComboBox->insertItems(0, m_profile->getserverList());
    ui->serverComboBox->setCurrentIndex(m_profile->getIndex());

    //desktop systray
    systrayMenu.addAction("Show/Hide", this, SLOT(showorhideWindow()));
    systrayMenu.addAction("Start/Stop", this, SLOT(startbuttonPressed()));
    systrayMenu.addAction("Exit", this, SLOT(close()));
    systray.setIcon(QIcon(":/icon/mono_icon.png"));
    systray.setToolTip(QString("Shadowsocks-Qt5"));
    systray.setContextMenu(&systrayMenu);
    systray.show();

    //Move to the center of the screen
    this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());
}

MainWindow::~MainWindow()
{
    ss_local.stop();//prevent crashes
    delete ui;
    delete m_profile;
}

void MainWindow::getSSLocalPath()
{
    ui->sslocalEdit->setText(QFileDialog::getOpenFileName());
}

void MainWindow::oncurrentProfileChanged(int i)
{
    if (i < 0) {//there is no profile
        addProfileDialogue(true);//enforce
        return;
    }

    ss_local.stop();
    m_profile->setIndex(i);
    current_profile = m_profile->getProfile(i);

    ui->serverComboBox->setCurrentText(current_profile.server);
    ui->serverPortEdit->setText(current_profile.server_port);
    ui->pwdEdit->setText(current_profile.password);
    ui->proxyPortEdit->setText(current_profile.local_port);
    ui->methodComboBox->setCurrentText(current_profile.method);
    ui->timeoutSpinBox->setValue(current_profile.timeout.toInt());
}

void MainWindow::addProfileDialogue(bool enforce = false)
{
    bool ok;
    QString server = QInputDialog::getText(this, this->windowTitle(), "Server IP or Domain", QLineEdit::Normal, NULL, &ok);
    if (ok) {
        m_profile->addProfile(server);
        current_profile = m_profile->lastProfile();
        ui->serverComboBox->insertItem(ui->serverComboBox->count(), server);

        //change serverComboBox, let it emit currentIndexChanged signal.
        ui->serverComboBox->setCurrentIndex(ui->serverComboBox->count() - 1);
    }
    else if (enforce) {
        m_profile->addProfile("");
        current_profile = m_profile->lastProfile();
        ui->serverComboBox->insertItem(ui->serverComboBox->count(), "");
        //since there was no item previously, serverComboBox would change itself automatically.
        //we don't need to emit the signal again.
    }
}

QString MainWindow::detectSSLocal()
{
    if (m_profile->app.isEmpty()) {
    #ifdef _WIN32
        QString bundled_sslocal = QCoreApplication::applicationDirPath() + "/ss-local.exe";
        if(QFile::exists(bundled_sslocal)) {
            return bundled_sslocal;
        }
        else
            return QString("");
    #endif
        return QStandardPaths::findExecutable("ss-local");
    }
    else
        return m_profile->app;
}

void MainWindow::savebuttonPressed()
{
    current_profile.server = ui->serverComboBox->currentText();
    current_profile.server_port = ui->serverPortEdit->text();
    current_profile.password = ui->pwdEdit->text();
    current_profile.local_port = ui->proxyPortEdit->text();
    current_profile.method = ui->methodComboBox->currentText();
    current_profile.timeout = ui->timeoutSpinBox->cleanText();
    m_profile->app = ui->sslocalEdit->text();
    m_profile->saveProfile(ui->serverComboBox->currentIndex(), current_profile);
    m_profile->saveProfileToJSON();
}

void MainWindow::startbuttonPressed()
{
    if ((QObjectUserData*)ui->startButton->userData(0) == 0) {
        ss_local.setapp(ui->sslocalEdit->text());
        ss_local.start(current_profile);
    }
    else {
        ss_local.stop();
    }
}

void MainWindow::showLogDialogue()
{
    logdlg.show();
    logdlg.exec();
}

void MainWindow::revertbuttonPressed()
{
    ss_local.stop();
    m_profile->revert();
    ui->sslocalEdit->setText(detectSSLocal());
    disconnect(ui->serverComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(oncurrentProfileChanged(int)));
    ui->serverComboBox->clear();
    connect(ui->serverComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(oncurrentProfileChanged(int)));
    ui->serverComboBox->insertItems(0, m_profile->getserverList());
    ui->serverComboBox->setCurrentIndex(m_profile->getIndex());
}

void MainWindow::deleteProfile()
{
    int i = ui->serverComboBox->currentIndex();
    m_profile->deleteProfile(i);
    ui->serverComboBox->removeItem(i);
}

void MainWindow::processStarted()
{
    ui->startButton->setUserData(0, (QObjectUserData *)1);
    ui->startButton->setText("Stop");
    ui->startButton->setIcon(QIcon::fromTheme("process-stop"));

    systray.setIcon(QIcon(":/icon/running_icon.png"));
}

void MainWindow::processStopped()
{
    ui->startButton->setUserData(0, (QObjectUserData *)0);
    ui->startButton->setText("Start");
    ui->startButton->setIcon(QIcon::fromTheme("run-build"));

    systray.setIcon(QIcon(":/icon/mono_icon.png"));
}

void MainWindow::showorhideWindow()
{
    if (this->isVisible()) {
        this->hide();
    }
    else {
        this->show();
        this->setWindowState(Qt::WindowActive);
        this->activateWindow();
        ui->logButton->setFocus();
    }
}

void MainWindow::systrayActivated(QSystemTrayIcon::ActivationReason r)
{
    if (r != 1) {
        showorhideWindow();
    }
}

void MainWindow::changeEvent(QEvent *e)
{
    if (e->type()==QEvent::WindowStateChange && this->isMinimized()) {
        this->hide();
    }
}
