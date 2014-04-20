#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //initialisation
#ifdef _WIN32
    jsonconfigFile = QCoreApplication::applicationDirPath() + "/gui-config.json";
#else
    QDir ssConfigDir = QDir::homePath() + "/.config/shadowsocks";
    jsonconfigFile = ssConfigDir.absolutePath() + "/gui-config.json";
    if (!ssConfigDir.exists()) {
        ssConfigDir.mkpath(ssConfigDir.absolutePath());
    }
#endif
    m_profile = new Profiles(jsonconfigFile);

    ui->profileComboBox->insertItems(0, m_profile->getProfileList());
    ui->stopButton->setEnabled(false);
    ui->laddrEdit->setValidator(&ipv4addrValidator);
    ui->sportEdit->setValidator(&portValidator);
    ui->lportEdit->setValidator(&portValidator);

    //desktop systray
    systrayMenu.addAction("Show/Hide", this, SLOT(showorhideWindow()));
    systrayMenu.addAction("Start", this, SLOT(startButtonPressed()));
    systrayMenu.addAction("Stop", this, SLOT(stopButtonPressed()));
    systrayMenu.addAction("Exit", this, SLOT(close()));
    systrayMenu.actions().at(2)->setEnabled(false);
#ifdef _WIN32
    systray.setIcon(QIcon(":/icon/black_icon.png"));
#else
    systray.setIcon(QIcon(":/icon/mono_icon.png"));
#endif
    systray.setToolTip(QString("Shadowsocks-Qt5"));
    systray.setContextMenu(&systrayMenu);
    systray.show();

    //Move to the center of the screen
    this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());

#ifdef _WIN32
    ui->logBrowser->setPlaceholderText(QString("Because of buffer, log print would be delayed."));
#endif

    //SIGNALs and SLOTs
    //connect(this, &MainWindow::currentProfileChanged, this, &MainWindow::onCurrentProfileChanged);
    connect(&ss_local, &SS_Process::readReadyProcess, this, &MainWindow::onReadReadyProcess);
    connect(&ss_local, &SS_Process::sigstart, this, &MainWindow::processStarted);
    connect(&ss_local, &SS_Process::sigstop, this, &MainWindow::processStopped);
    connect(&systray, &QSystemTrayIcon::activated, this, &MainWindow::systrayActivated);

    connect(ui->backendToolButton, &QToolButton::clicked, this, &MainWindow::onBackendToolButtonPressed);
    connect(ui->profileComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::onCurrentProfileChanged);
    connect(ui->profileComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::highlighted), this, &MainWindow::onProfileComboBoxActivated);
    connect(ui->backendTypeCombo, &QComboBox::currentTextChanged, this, &MainWindow::backendTypeChanged);
    connect(ui->addProfileButton, &QToolButton::clicked, this, &MainWindow::addProfileDialogue);
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::startButtonPressed);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::stopButtonPressed);
    connect(ui->delProfileButton, &QToolButton::clicked, this, &MainWindow::deleteProfile);

    //update current configuration
    ui->profileComboBox->setCurrentIndex(m_profile->getIndex());
    ui->backendTypeCombo->setCurrentText(m_profile->getBackendType());
    /*
     * If there is only one config in gui-config.json, then the function above wouldn't emit signal.
     * Therefore, we have to emit a signal manually.
     */
    emit ui->profileComboBox->currentIndexChanged(m_profile->getIndex());
    emit ui->backendTypeCombo->currentTextChanged(m_profile->getBackendType());

    //connect signals and slots when config changed
    //Profile
    connect(this, &MainWindow::configurationChanged, this, &MainWindow::onConfigurationChanged);
    connect(ui->serverEdit, &QLineEdit::textChanged, this, &MainWindow::serverEditFinished);
    connect(ui->sportEdit, &QLineEdit::textChanged, this, &MainWindow::sportEditFinished);
    connect(ui->pwdEdit, &QLineEdit::textChanged, this, &MainWindow::pwdEditFinished);
    connect(ui->laddrEdit, &QLineEdit::textChanged, this, &MainWindow::laddrEditFinished);
    connect(ui->lportEdit, &QLineEdit::textChanged, this, &MainWindow::lportEditFinished);
    connect(ui->methodComboBox, &QComboBox::currentTextChanged, this, &MainWindow::methodChanged);
    connect(ui->timeoutSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::timeoutChanged);
    connect(ui->profileEditButtonBox, &QDialogButtonBox::clicked, this, &MainWindow::profileEditButtonClicked);

    //Misc
    connect(this, &MainWindow::miscConfigurationChanged, this, &MainWindow::onMiscConfigurationChanged);
    connect(ui->autohideCheck, &QCheckBox::stateChanged, this, &MainWindow::autoHideChecked);
    connect(ui->autostartCheck, &QCheckBox::stateChanged, this, &MainWindow::autoStartChecked);
    connect(ui->debugCheck, &QCheckBox::stateChanged, this, &MainWindow::debugChecked);
    connect(ui->miscSaveButton, &QPushButton::clicked, this, &MainWindow::miscButtonBoxClicked);
    connect(ui->aboutButton, &QPushButton::clicked, this, &MainWindow::aboutButtonClicked);
}

MainWindow::~MainWindow()
{
    ss_local.stop();//prevent crashes
    delete ui;
    delete m_profile;
}

void MainWindow::onBackendToolButtonPressed()
{
    QString backend = QFileDialog::getOpenFileName();
    if (!backend.isEmpty()) {
        m_profile->setBackend(backend);
        ui->backendEdit->setText(m_profile->getBackend());
        emit configurationChanged();
    }
    this->setWindowState(Qt::WindowActive);
    this->activateWindow();
    ui->backendEdit->setFocus();
}

void MainWindow::backendTypeChanged(const QString &type)
{
    m_profile->setBackendType(type);

    //detect backend again no matter empty or not
    ui->backendEdit->setText(detectSSLocal());

    emit configurationChanged();
}

void MainWindow::onProfileComboBoxActivated(int i)
{
    if (i != ui->profileComboBox->currentIndex()) {//user was trying to change profile
        checkIfSaved();
    }
}

void MainWindow::onCurrentProfileChanged(int i)
{
    if (i < 0) {//there is no profile
        addProfileDialogue(true);//enforce
        return;
    }

    ss_local.stop();//Q: should we stop the backend when profile changed?
    m_profile->setIndex(i);
    current_profile = m_profile->getProfile(i);

    ui->serverEdit->setText(current_profile.server);
    ui->sportEdit->setText(current_profile.server_port);
    ui->pwdEdit->setText(current_profile.password);
    ui->laddrEdit->setText(current_profile.local_addr);
    ui->lportEdit->setText(current_profile.local_port);
    ui->methodComboBox->setCurrentText(current_profile.method);
    ui->timeoutSpinBox->setValue(current_profile.timeout.toInt());
    ui->debugCheck->setChecked(m_profile->isDebug());
    ui->autohideCheck->setChecked(m_profile->isAutoHide());
    ui->autostartCheck->setChecked(m_profile->isAutoStart());

    emit configurationChanged();
}

void MainWindow::addProfileDialogue(bool enforce = false)
{
    bool ok;
    QString profile = QInputDialog::getText(this, this->windowTitle(), "Profile Name", QLineEdit::Normal, NULL, &ok);
    if (ok) {
        m_profile->addProfile(profile);
        current_profile = m_profile->lastProfile();
        ui->profileComboBox->insertItem(ui->profileComboBox->count(), profile);

        //change serverComboBox, let it emit currentIndexChanged signal.
        ui->profileComboBox->setCurrentIndex(ui->profileComboBox->count() - 1);
    }
    else if (enforce) {
        m_profile->addProfile("");
        current_profile = m_profile->lastProfile();
        ui->profileComboBox->insertItem(ui->profileComboBox->count(), "");
        //since there was no item previously, serverComboBox would change itself automatically.
        //we don't need to emit the signal again.
    }
}

QString MainWindow::detectSSLocal()
{
    /*
     * TODO
     * detect whether current backend type matches the one user selected
     * if not, then detect backend from PATH. otherwise keep the old value.
     */
    QString execName, sslocal;
    switch (m_profile->getBackendTypeID()) {
    case 1:
        execName = "sslocal";
        break;
    default:
        execName = "ss-local";
    }

#ifdef _WIN32
    sslocal = QCoreApplication::applicationDirPath() + "/" + execName;
    if(!QFile::exists(sslocal)) {
        sslocal = QStandardPaths::findExecutable(execName);
    }
#else
    sslocal = QStandardPaths::findExecutable(execName);
#endif
    if(!sslocal.isEmpty()) {
        m_profile->setBackend(sslocal);
    }

    return m_profile->getBackend();
}

void MainWindow::saveProfile()
{
    m_profile->saveProfile(ui->profileComboBox->currentIndex(), current_profile);
    m_profile->saveProfileToJSON();
    emit onConfigurationChanged(true);
    emit miscConfigurationChanged(true);
}

void MainWindow::saveMiscConfig()
{
    m_profile->saveProfileToJSON();
    emit miscConfigurationChanged(true);
    emit onConfigurationChanged(true);
}

void MainWindow::profileEditButtonClicked(QAbstractButton *b)
{
    if (ui->profileEditButtonBox->standardButton(b) == QDialogButtonBox::Save) {
        saveProfile();
    }
    else {//reset
        m_profile->revert();
        ui->backendTypeCombo->setCurrentText(m_profile->getBackendType());
        disconnect(ui->profileComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::onCurrentProfileChanged);
        ui->profileComboBox->clear();
        ui->profileComboBox->insertItems(0, m_profile->getProfileList());
        ui->profileComboBox->setCurrentIndex(m_profile->getIndex());
        connect(ui->profileComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::onCurrentProfileChanged);
        emit ui->profileComboBox->currentIndexChanged(m_profile->getIndex());//same in MainWindow's constructor
        emit onConfigurationChanged(true);
    }
}

void MainWindow::startButtonPressed()
{
    checkIfSaved();

    if (!m_profile->isValidate(current_profile)) {
        QMessageBox::critical(this, "Error", "Invalid profile or configuration.");
        return;
    }

    ss_local.setapp(m_profile->getBackend());
    ss_local.setTypeID(m_profile->getBackendTypeID());
    ss_local.start(current_profile);
}

void MainWindow::stopButtonPressed()
{
    ss_local.stop();
}

void MainWindow::deleteProfile()
{
    int i = ui->profileComboBox->currentIndex();
    m_profile->deleteProfile(i);
    ui->profileComboBox->removeItem(i);
}

void MainWindow::processStarted()
{
    systrayMenu.actions().at(1)->setEnabled(false);
    systrayMenu.actions().at(2)->setEnabled(true);
    ui->stopButton->setEnabled(true);
    ui->startButton->setEnabled(false);
    ui->logBrowser->clear();

    systray.setIcon(QIcon(":/icon/running_icon.png"));
}

void MainWindow::processStopped()
{
    systrayMenu.actions().at(1)->setEnabled(true);
    systrayMenu.actions().at(2)->setEnabled(false);
    ui->stopButton->setEnabled(false);
    ui->startButton->setEnabled(true);

#ifdef _WIN32
    systray.setIcon(QIcon(":/icon/black_icon.png"));
#else
    systray.setIcon(QIcon(":/icon/mono_icon.png"));
#endif
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
        ui->startButton->setFocus();
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

void MainWindow::onReadReadyProcess(const QByteArray &o)
{
    ui->logBrowser->moveCursor(QTextCursor::End);
    ui->logBrowser->append(o);
    ui->logBrowser->moveCursor(QTextCursor::End);
}

void MainWindow::onConfigurationChanged(bool saved)
{
    ui->profileEditButtonBox->setEnabled(!saved);
}

void MainWindow::onMiscConfigurationChanged(bool saved)
{
    ui->miscSaveButton->setEnabled(!saved);
}

void MainWindow::serverEditFinished(const QString &str)
{
    current_profile.server = str;
    emit configurationChanged();
}

void MainWindow::sportEditFinished(const QString &str)
{
    current_profile.server_port = str;
    emit configurationChanged();
}

void MainWindow::pwdEditFinished(const QString &str)
{
    current_profile.password = str;
    emit configurationChanged();
}

void MainWindow::laddrEditFinished(const QString &str)
{
    current_profile.local_addr = str;
    emit configurationChanged();
}

void MainWindow::lportEditFinished(const QString &str)
{
    current_profile.local_port = str;
    emit configurationChanged();
}

void MainWindow::methodChanged(const QString &m)
{
    current_profile.method = m;
    emit configurationChanged();
}

void MainWindow::timeoutChanged(int t)
{
    current_profile.timeout = QString::number(t);
    emit configurationChanged();
}

void MainWindow::autoHideChecked(int c)
{
    if (c == Qt::Checked) {
        m_profile->setAutoHide(true);
    }
    else
        m_profile->setAutoHide(false);

    emit miscConfigurationChanged();
}

void MainWindow::autoStartChecked(int c)
{
    if (c == Qt::Checked) {
        m_profile->setAutoStart(true);
    }
    else
        m_profile->setAutoStart(false);

    emit miscConfigurationChanged();
}

void MainWindow::debugChecked(int c)
{
    if (c == Qt::Checked) {
        m_profile->setDebug(true);
    }
    else
        m_profile->setDebug(false);

    emit miscConfigurationChanged();
}

void MainWindow::miscButtonBoxClicked()
{
    saveMiscConfig();
}

void MainWindow::checkIfSaved()
{
    if (ui->profileEditButtonBox->isEnabled()) {
        QMessageBox::StandardButton save = QMessageBox::question(this, "Unsaved Profile", "Current profile is not saved yet.\nDo you want to save it now?", QMessageBox::Save|QMessageBox::No, QMessageBox::Save);
        if (save == QMessageBox::Save) {
            saveProfile();
        }
        else {
            emit configurationChanged(true);
        }
    }
}

void MainWindow::aboutButtonClicked()
{
    QMessageBox::about(this, QString("About Shadowsocks-Qt5"), QString("<h3>Platform-Cross GUI Client for Shadowsocks.</h3><p>Version: 0.3.0</p><p>Copyright © 2014 William Wong</p><p>Licensed under LGPLv3<br />Project Hosted at <a href='https://github.com/librehat/shadowsocks-qt5'>GitHub</a>.</p>"));
}
