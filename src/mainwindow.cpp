#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifdef _WIN32
#include <QtWin>
#endif

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
    m_conf = new Configuration(jsonconfigFile);

    ui->profileComboBox->insertItems(0, m_conf->getProfileList());
    ui->stopButton->setEnabled(false);
    ui->laddrEdit->setValidator(&ipv4addrValidator);
    ui->sportEdit->setValidator(&portValidator);
    ui->lportEdit->setValidator(&portValidator);
    ui->methodComboBox->addItems(SSValidator::supportedMethod);

    ui->debugCheck->setChecked(m_conf->isDebug());
    ui->autohideCheck->setChecked(m_conf->isAutoHide());
    ui->autostartCheck->setChecked(m_conf->isAutoStart());
#ifdef __linux__
    ui->translucentCheck->setVisible(false);
#else
    ui->translucentCheck->setChecked(m_conf->isTranslucent());
    updateTranslucent(m_conf->isTranslucent());
#endif
    ui->relativePathCheck->setChecked(m_conf->isRelativePath());

    //desktop systray
    systrayMenu.addAction(tr("Show"), this, SLOT(showWindow()));
    systrayMenu.addAction(tr("Start"), this, SLOT(startButtonPressed()));
    systrayMenu.addAction(tr("Stop"), this, SLOT(stopButtonPressed()));
    systrayMenu.addAction(tr("Exit"), this, SLOT(close()));
    systrayMenu.actions().at(2)->setEnabled(false);
#ifdef _WIN32
    systray.setIcon(QIcon(":/icon/black_icon.png"));
#else
    systray.setIcon(QIcon(":/icon/mono_icon.png"));
#endif
    systray.setToolTip(QString("Shadowsocks-Qt5"));
    systray.setContextMenu(&systrayMenu);
    systray.show();

    //Windows Extras
#ifdef _WIN32
    QtWin::enableBlurBehindWindow(this);
    QtWin::extendFrameIntoClientArea(this, -1, -1, -1, -1);
    QtWin::enableBlurBehindWindow(&addProfileDlg);
    QtWin::extendFrameIntoClientArea(&addProfileDlg, -1, -1, -1, -1);
    //smaller margins
    ui->verticalLayout->setMargin(4);//centralwidget
#endif

    //Move to the center of the screen
    this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());

    //SIGNALs and SLOTs
    connect(&ss_local, &SS_Process::readReadyProcess, this, &MainWindow::onReadReadyProcess);
    connect(&ss_local, &SS_Process::sigstart, this, &MainWindow::processStarted);
    connect(&ss_local, &SS_Process::sigstop, this, &MainWindow::processStopped);
    connect(&systray, &QSystemTrayIcon::activated, this, &MainWindow::systrayActivated);

    connect(ui->backendToolButton, &QToolButton::clicked, this, &MainWindow::onBackendToolButtonPressed);
    connect(ui->profileComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::onCurrentProfileChanged);
    connect(ui->backendTypeCombo, &QComboBox::currentTextChanged, this, &MainWindow::backendTypeChanged);
    connect(ui->addProfileButton, &QToolButton::clicked, this, &MainWindow::addProfileDialogue);
    connect(ui->delProfileButton, &QToolButton::clicked, this, &MainWindow::deleteProfile);
    connect(&addProfileDlg, &AddProfileDialogue::inputAccepted, this, &MainWindow::onAddProfileDialogueAccepted);
    connect(&addProfileDlg, &AddProfileDialogue::inputRejected, this, &MainWindow::onAddProfileDialogueRejected);
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::startButtonPressed);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::stopButtonPressed);

    //update current configuration
    ui->profileComboBox->setCurrentIndex(m_conf->getIndex());
    /*
     * If there is no gui-config file, or the index is 0, then the function above wouldn't emit signal.
     * Therefore, we have to emit a signal manually.
     */
    if (m_conf->getIndex() <= 0) {
        emit ui->profileComboBox->currentIndexChanged(m_conf->getIndex());
    }

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
#ifdef __linux__
    connect(ui->tfoCheckBox, &QCheckBox::toggled, this, &MainWindow::tcpFastOpenChanged);
#endif
    connect(ui->profileEditButtonBox, &QDialogButtonBox::clicked, this, &MainWindow::profileEditButtonClicked);

    //Misc
    connect(ui->autohideCheck, &QCheckBox::stateChanged, this, &MainWindow::autoHideToggled);
    connect(ui->autostartCheck, &QCheckBox::stateChanged, this, &MainWindow::autoStartToggled);
    connect(ui->debugCheck, &QCheckBox::stateChanged, this, &MainWindow::debugToggled);
    connect(ui->translucentCheck, &QCheckBox::toggled, this, &MainWindow::transculentToggled);
    connect(ui->relativePathCheck, &QCheckBox::toggled, this, &MainWindow::relativePathToggled);
    connect(ui->miscSaveButton, &QPushButton::clicked, this, &MainWindow::saveConfig);
    connect(ui->aboutButton, &QPushButton::clicked, this, &MainWindow::aboutButtonClicked);
}

MainWindow::~MainWindow()
{
    ss_local.stop();//prevent crashes
    delete ui;
    delete m_conf;
}

const QString MainWindow::aboutText = "<h3>Cross-Platform GUI Fronted for Shadowsocks</h3><p>Version: 0.4.3</p><p>Copyright © 2014 Symeon Huang (<a href='https://twitter.com/librehat'>@librehat</a>)</p><p>Licensed under LGPLv3<br />Project Hosted at <a href='https://github.com/librehat/shadowsocks-qt5'>GitHub</a></p>";

void MainWindow::onBackendToolButtonPressed()
{
    QString backend = QFileDialog::getOpenFileName();
    if (!backend.isEmpty()) {
        current_profile->setBackend(backend, m_conf->isRelativePath());
        ui->backendEdit->setText(current_profile->backend);
        emit configurationChanged();
    }
    this->setWindowState(Qt::WindowActive);
    this->activateWindow();
    ui->serverEdit->setFocus();
}

void MainWindow::onCurrentProfileChanged(int i)
{
    if (i < 0) {//there is no profile
        addProfileDialogue(true);//enforce
        return;
    }

    ss_local.stop();//Q: should we stop the backend when profile changed?
    m_conf->setIndex(i);
    current_profile = m_conf->currentProfile();
    if (current_profile->backend.isEmpty()) {
        current_profile->setBackend(m_conf->isRelativePath());
        if (!current_profile->backend.isEmpty()) {
            ui->profileEditButtonBox->setEnabled(true);
            ui->miscSaveButton->setEnabled(true);
        }
    }

    ui->backendTypeCombo->setCurrentIndex(current_profile->getBackendTypeID());
    ui->backendEdit->setText(current_profile->backend);
    ui->serverEdit->setText(current_profile->server);
    ui->sportEdit->setText(current_profile->server_port);
    ui->pwdEdit->setText(current_profile->password);
    ui->laddrEdit->setText(current_profile->local_addr);
    ui->lportEdit->setText(current_profile->local_port);
    ui->methodComboBox->setCurrentText(current_profile->method);
    ui->timeoutSpinBox->setValue(current_profile->timeout.toInt());
#ifdef __linux__
    ui->tfoCheckBox->setChecked(current_profile->fast_open);
#endif

    emit configurationChanged();
}

void MainWindow::addProfileDialogue(bool enforce = false)
{
    addProfileDlg.setEnforceMode(enforce);
    addProfileDlg.clear();
    addProfileDlg.show();
    addProfileDlg.exec();
}

void MainWindow::onAddProfileDialogueAccepted(const QString &name, bool u, const QString &uri)
{
    if(u) {
        m_conf->addProfileFromSSURI(name, uri);
    }
    else {
        m_conf->addProfile(name);
    }
    current_profile = m_conf->lastProfile();
    ui->profileComboBox->insertItem(ui->profileComboBox->count(), current_profile->profileName);

    //change serverComboBox, let it emit currentIndexChanged signal.
    ui->profileComboBox->setCurrentIndex(ui->profileComboBox->count() - 1);
}

void MainWindow::onAddProfileDialogueRejected(bool enforce)
{
    if (enforce) {
        m_conf->addProfile("");
        current_profile = m_conf->lastProfile();
        ui->profileComboBox->insertItem(ui->profileComboBox->count(), "");
        //since there was no item previously, serverComboBox would change itself automatically.
        //we don't need to emit the signal again.
    }
}

void MainWindow::saveConfig()
{
    m_conf->save();
    emit configurationChanged(true);
}

void MainWindow::minimizeToSysTray()
{
#ifdef __linux__
    QString currentDE = (getenv("XDG_CURRENT_DESKTOP"));
    /*
     * There is no systray in Unity. Nor a clean and simple way to use the indicator.
     * While hiding window will cause inability to restore.
     * Simply not to hide if the desktop is Unity.
     */
    if (currentDE.compare("Unity") == 0) {
        return;
    }
#endif
    this->hide();
}

void MainWindow::profileEditButtonClicked(QAbstractButton *b)
{
    if (ui->profileEditButtonBox->standardButton(b) == QDialogButtonBox::Save) {
        saveConfig();
    }
    else {//reset
        m_conf->revert();
        disconnect(ui->profileComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::onCurrentProfileChanged);
        ui->profileComboBox->clear();
        ui->profileComboBox->insertItems(0, m_conf->getProfileList());
        ui->profileComboBox->setCurrentIndex(m_conf->getIndex());
        connect(ui->profileComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::onCurrentProfileChanged);
        emit ui->profileComboBox->currentIndexChanged(m_conf->getIndex());//same in MainWindow's constructor
        emit configurationChanged(true);
    }
}

void MainWindow::startButtonPressed()
{
    if (!current_profile->isValid()) {
        QMessageBox::critical(this, tr("Error"), tr("Invalid profile or configuration."));
        return;
    }

    ss_local.start(current_profile);
}

void MainWindow::showNotification(QString msg)
{
    systray.showMessage("Shadowsocks-Qt5", msg);
}

void MainWindow::deleteProfile()
{
    int i = ui->profileComboBox->currentIndex();
    m_conf->deleteProfile(i);
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
    showNotification(tr("Shadowsocks started! profile: %1.").arg(current_profile->profileName));
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

    showNotification(tr("Shadowsocks stoped!"));
}

void MainWindow::showWindow()
{
    this->show();
    this->setWindowState(Qt::WindowActive);
    this->activateWindow();
    ui->startButton->setFocus();
}

void MainWindow::systrayActivated(QSystemTrayIcon::ActivationReason r)
{
    if (r != 1) {
        showWindow();
    }
}

void MainWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::WindowStateChange && this->isMinimized()) {
        minimizeToSysTray();
    }
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (ui->profileEditButtonBox->isEnabled()) {//which means unsaved
        QMessageBox::StandardButton answer = QMessageBox::question(this, tr("Save Changes"), tr("Configuration has been changed.\nDo you want to save it now?"), QMessageBox::Cancel|QMessageBox::Save|QMessageBox::No, QMessageBox::Save);
        if (answer == QMessageBox::Cancel) {
            e->ignore();
            return;
        }
        else if (answer == QMessageBox::Save) {
            saveConfig();
        }
    }
    QWidget::closeEvent(e);
}

void MainWindow::onReadReadyProcess(const QByteArray &o)
{
    ui->logBrowser->moveCursor(QTextCursor::End);
    ui->logBrowser->append(QString::fromLocal8Bit(o));
    ui->logBrowser->moveCursor(QTextCursor::End);
}

void MainWindow::onConfigurationChanged(bool saved)
{
    ui->profileEditButtonBox->setEnabled(!saved);
    ui->miscSaveButton->setEnabled(!saved);
}

void MainWindow::backendTypeChanged(const QString &type)
{
    current_profile->type = type;

    ui->backendEdit->setText(current_profile->getBackend());

    int tID = current_profile->getBackendTypeID();
    if (tID == 2) {//shadowsocks-go doesn't support timeout argument
        ui->timeoutSpinBox->setVisible(false);
        ui->timeoutLabel->setVisible(false);
    }
    else {
        ui->timeoutSpinBox->setVisible(true);
        ui->timeoutLabel->setVisible(true);
    }
    if ((tID == 0 || tID == 3) && m_conf->isTFOAvailable()) {
        ui->tfoCheckBox->setVisible(true);
    }
    else {
        ui->tfoCheckBox->setVisible(false);
    }
    emit configurationChanged();
}

void MainWindow::serverEditFinished(const QString &str)
{
    current_profile->server = str;
    emit configurationChanged();
}

void MainWindow::sportEditFinished(const QString &str)
{
    current_profile->server_port = str;
    emit configurationChanged();
}

void MainWindow::pwdEditFinished(const QString &str)
{
    current_profile->password = str;
    emit configurationChanged();
}

void MainWindow::laddrEditFinished(const QString &str)
{
    current_profile->local_addr = str;
    emit configurationChanged();
}

void MainWindow::lportEditFinished(const QString &str)
{
    current_profile->local_port = str;
    emit configurationChanged();
}

void MainWindow::methodChanged(const QString &m)
{
    current_profile->method = m;
    emit configurationChanged();
}

void MainWindow::timeoutChanged(int t)
{
    current_profile->timeout = QString::number(t);
    emit configurationChanged();
}

#ifdef __linux__
void MainWindow::tcpFastOpenChanged(bool t)
{
    current_profile->fast_open = t;
    emit configurationChanged();
}
#endif

void MainWindow::autoHideToggled(bool c)
{
    m_conf->setAutoHide(c);
    emit configurationChanged();
}

void MainWindow::autoStartToggled(bool c)
{
    m_conf->setAutoStart(c);
    emit configurationChanged();
}

void MainWindow::debugToggled(bool c)
{
    m_conf->setDebug(c);
    emit configurationChanged();
}

void MainWindow::transculentToggled(bool c)
{
    m_conf->setTranslucent(c);
    emit configurationChanged();
}

void MainWindow::updateTranslucent(bool translucent)
{
    /*
     * don't use translucent background for Linux!
     */
#ifndef __linux__
    if (translucent) {
        this->setAttribute(Qt::WA_TranslucentBackground);
        addProfileDlg.setAttribute(Qt::WA_TranslucentBackground);
    }
#else
    Q_UNUSED(translucent);
#endif
}

void MainWindow::relativePathToggled(bool r)
{
    m_conf->setRelativePath(r);
    emit configurationChanged();
}
