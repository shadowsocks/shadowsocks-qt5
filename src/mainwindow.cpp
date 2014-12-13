#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sharedialogue.h"

#ifdef Q_OS_WIN
#include <QtWin>
#endif

#ifdef Q_OS_LINUX
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusPendingCall>
#endif

MainWindow::MainWindow(bool verbose, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //initialisation
    verboseOutput = verbose;
    if (verboseOutput) {
        qDebug() << "Verbose Enabled.";
    }
#ifdef Q_OS_WIN
    jsonconfigFile = QCoreApplication::applicationDirPath() + "/gui-config.json";
#else
    QDir ssConfigDir = QDir::homePath() + "/.config/shadowsocks";
    jsonconfigFile = ssConfigDir.absolutePath() + "/gui-config.json";
    if (!ssConfigDir.exists()) {
        ssConfigDir.mkpath(ssConfigDir.absolutePath());
    }
#endif
    m_conf = new Configuration(jsonconfigFile);
    ssProcess = new SS_Process(this);

    ui->laddrEdit->setValidator(&ipv4addrValidator);
    ui->lportEdit->setValidator(&portValidator);
    ui->methodComboBox->addItems(SSValidator::supportedMethod);
    ui->profileComboBox->addItems(m_conf->getProfileList());
    ui->sportEdit->setValidator(&portValidator);
    ui->stopButton->setEnabled(false);

    ui->autohideCheck->setChecked(m_conf->isAutoHide());
    ui->autostartCheck->setChecked(m_conf->isAutoStart());
    ui->debugCheck->setChecked(m_conf->isDebug());
#ifdef Q_OS_LINUX
    ui->translucentCheck->setVisible(false);
#else
    ui->translucentCheck->setChecked(m_conf->isTranslucent());
    if(m_conf->isTranslucent()) {
        this->setAttribute(Qt::WA_TranslucentBackground);
    }
    ui->tfoCheckBox->setVisible(false);
#endif
    ui->relativePathCheck->setChecked(m_conf->isRelativePath());

    //desktop systray
    systrayMenu.addAction(tr("Show"), this, SLOT(showWindow()));
    systrayMenu.addAction(tr("Start"), this, SLOT(onStartButtonPressed()));
    systrayMenu.addAction(tr("Stop"), this, SLOT(onStopButtonPressed()));
    systrayMenu.addAction(tr("Exit"), this, SLOT(close()));
    systrayMenu.actions().at(2)->setEnabled(false);
#ifdef Q_OS_WIN
    systray.setIcon(QIcon(":/icon/black_icon.png"));
#else
    systray.setIcon(QIcon(":/icon/mono_icon.png"));
#endif
    systray.setToolTip(QString("Shadowsocks-Qt5"));
    systray.setContextMenu(&systrayMenu);
#ifdef Q_OS_LINUX
    isUbuntuUnity = (QString(getenv("XDG_CURRENT_DESKTOP")).compare("Unity", Qt::CaseInsensitive) == 0);
    if (!isUbuntuUnity) {
        systray.show();
    }
#else
    systray.show();
#endif

    //Windows Extras
#ifdef Q_OS_WIN
    QtWin::enableBlurBehindWindow(this);
    QtWin::extendFrameIntoClientArea(this, -1, -1, -1, -1);
    //smaller margins
    ui->verticalLayout->setMargin(4);//centralwidget
#endif

    //Move to the center of the screen
    this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());

    /*
     * SIGNALs and SLOTs
     */
    connect(ssProcess, &SS_Process::processRead, this, &MainWindow::onProcessReadyRead);
    connect(ssProcess, &SS_Process::processStarted, this, &MainWindow::onProcessStarted);
    connect(ssProcess, &SS_Process::processStopped, this, &MainWindow::onProcessStopped);
    connect(&systray, &QSystemTrayIcon::activated, this, &MainWindow::systrayActivated);

    connect(ui->backendToolButton, &QToolButton::clicked, this, &MainWindow::onBackendToolButtonPressed);

    /*
     * I believe there's bug. see https://bugreports.qt-project.org/browse/QTBUG-41863
     * before it's fixed, let's use old way to work around.
     *
    connect(ui->profileComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::onCurrentProfileChanged);
    */
    connect(ui->profileComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentProfileChanged(int)));

    connect(ui->backendTypeCombo, &QComboBox::currentTextChanged, this, &MainWindow::onBackendTypeChanged);
    connect(ui->addProfileButton, &QToolButton::clicked, this, &MainWindow::addProfileDialogue);
    connect(ui->delProfileButton, &QToolButton::clicked, this, &MainWindow::deleteProfile);
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartButtonPressed);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onStopButtonPressed);
    connect(ui->shareButton, &QPushButton::clicked, this, &MainWindow::onShareButtonClicked);

    connect(this, &MainWindow::configurationChanged, this, &MainWindow::onConfigurationChanged);
    connect(ui->customArgEdit, &QLineEdit::textChanged, this, &MainWindow::onCustomArgsEditFinished);
    connect(ui->laddrEdit, &QLineEdit::textChanged, this, &MainWindow::onLAddrEditFinished);
    connect(ui->lportEdit, &QLineEdit::textChanged, this, &MainWindow::onLPortEditFinished);
    connect(ui->methodComboBox, &QComboBox::currentTextChanged, this, &MainWindow::onMethodChanged);
    connect(ui->pwdEdit, &QLineEdit::textChanged, this, &MainWindow::onPasswordEditFinished);
    connect(ui->serverEdit, &QLineEdit::textChanged, this, &MainWindow::onServerEditFinished);
    connect(ui->sportEdit, &QLineEdit::textChanged, this, &MainWindow::onSPortEditFinished);
    connect(ui->timeoutSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::onTimeoutChanged);
#ifdef Q_OS_LINUX
    connect(ui->tfoCheckBox, &QCheckBox::toggled, this, &MainWindow::onTcpFastOpenChanged);
#endif
    connect(ui->profileEditButtonBox, &QDialogButtonBox::clicked, this, &MainWindow::onProfileEditButtonClicked);

    connect(ui->autohideCheck, &QCheckBox::stateChanged, this, &MainWindow::onAutoHideToggled);
    connect(ui->autostartCheck, &QCheckBox::stateChanged, this, &MainWindow::onAutoStartToggled);
    connect(ui->debugCheck, &QCheckBox::stateChanged, this, &MainWindow::onDebugToggled);
    connect(ui->translucentCheck, &QCheckBox::toggled, this, &MainWindow::onTransculentToggled);
    connect(ui->relativePathCheck, &QCheckBox::toggled, this, &MainWindow::onRelativePathToggled);
    connect(ui->miscSaveButton, &QPushButton::clicked, this, &MainWindow::saveConfig);
    connect(ui->aboutButton, &QPushButton::clicked, this, &MainWindow::onAboutButtonClicked);

    //update current configuration
    ui->profileComboBox->setCurrentIndex(m_conf->getIndex());
    /*
     * If there is no gui-config file, or the index is 0, then the function above wouldn't emit signal.
     * Therefore, we have to emit a signal manually.
     */
    if (m_conf->getIndex() <= 0) {
        emit ui->profileComboBox->currentIndexChanged(m_conf->getIndex());
    }
    //finishing UI initialisation
    onBackendTypeChanged(ui->backendTypeCombo->currentText());
    emit configurationChanged(true);
}

MainWindow::~MainWindow()
{
    ssProcess->stop();//prevent crashes
    delete ui;
    delete m_conf;
}

const QString MainWindow::aboutText = "<h3>Cross-Platform GUI Fronted for Shadowsocks</h3><p>Version: " + QString(APP_VERSION) + "</p><p>Copyright © 2014 Symeon Huang (<a href='https://twitter.com/librehat'>@librehat</a>)</p><p>Licensed under LGPLv3<br />Project Hosted at <a href='https://github.com/librehat/shadowsocks-qt5'>GitHub</a></p>";

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

    /*
     * block all children signals temporarily.
     * avoid false configurationChanged() signals emiited.
     */
    blockChildrenSignals(true);

    ssProcess->stop();//Q: should we stop the backend when profile changed?
    if(i != m_conf->getIndex()) {
        emit configurationChanged();
    }
    m_conf->setIndex(i);
    current_profile = m_conf->currentProfile();
    if (current_profile->backend.isEmpty()) {
        current_profile->setBackend(m_conf->isRelativePath());
        if (!current_profile->backend.isEmpty()) {
            ui->profileEditButtonBox->setEnabled(true);
            ui->miscSaveButton->setEnabled(true);
        }
    }

    ui->backendEdit->setText(current_profile->backend);
    ui->backendTypeCombo->setCurrentIndex(current_profile->getBackendType());//the enum int is the same index in backend type combo box
    ui->customArgEdit->setText(current_profile->custom_arg);
    ui->laddrEdit->setText(current_profile->local_addr);
    ui->lportEdit->setText(current_profile->local_port);
    ui->methodComboBox->setCurrentText(current_profile->method);
    ui->pwdEdit->setText(current_profile->password);
    ui->serverEdit->setText(current_profile->server);
    ui->sportEdit->setText(current_profile->server_port);
    ui->timeoutSpinBox->setValue(current_profile->timeout.toInt());
#ifdef Q_OS_LINUX
    ui->tfoCheckBox->setChecked(current_profile->fast_open);
#endif

    blockChildrenSignals(false);
}

void MainWindow::onCustomArgsEditFinished(const QString &arg)
{
    current_profile->custom_arg = arg;
    emit configurationChanged();
}

void MainWindow::onShareButtonClicked()
{
    ShareDialogue *shareDlg = new ShareDialogue(current_profile->getSsUrl(), this);
    shareDlg->exec();
}

void MainWindow::addProfileDialogue(bool enforce = false)
{
    addProfileDlg = new AddProfileDialogue(this, enforce);
    connect(addProfileDlg, &AddProfileDialogue::inputAccepted, this, &MainWindow::onAddProfileDialogueAccepted);
    connect(addProfileDlg, &AddProfileDialogue::inputRejected, this, &MainWindow::onAddProfileDialogueRejected);
#ifdef Q_OS_WIN
    QtWin::enableBlurBehindWindow(addProfileDlg);
    QtWin::extendFrameIntoClientArea(addProfileDlg, -1, -1, -1, -1);
#endif
#ifndef Q_OS_LINUX
    if (m_conf->isTranslucent()) {
        addProfileDlg->setAttribute(Qt::WA_TranslucentBackground);
    }
#endif
    addProfileDlg->exec();
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
        m_conf->addProfile("Unnamed");
        current_profile = m_conf->lastProfile();
        ui->profileComboBox->insertItem(ui->profileComboBox->count(), "Unnamed");
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
#ifdef Q_OS_LINUX
    /*
     * There is no systray in Unity. Nor a clean and simple way to use the indicator.
     * While hiding window will cause inability to restore.
     * Simply not to hide if the desktop is Unity.
     */
    if (isUbuntuUnity) {
        return;
    }
#endif
    this->hide();
}

void MainWindow::onProfileEditButtonClicked(QAbstractButton *b)
{
    if (ui->profileEditButtonBox->standardButton(b) == QDialogButtonBox::Save) {
        saveConfig();
    }
    else {//reset
        m_conf->revert();
        this->blockChildrenSignals(true);
        ui->profileComboBox->clear();
        ui->profileComboBox->insertItems(0, m_conf->getProfileList());
        ui->profileComboBox->setCurrentIndex(m_conf->getIndex());
        this->blockChildrenSignals(false);
        emit ui->profileComboBox->currentIndexChanged(m_conf->getIndex());//same in MainWindow's constructor
        emit configurationChanged(true);
    }
}

void MainWindow::onStartButtonPressed()
{
    if (!current_profile->isValid()) {
        QMessageBox::critical(this, tr("Error"), tr("Invalid profile or configuration."));
        return;
    }

    ssProcess->start(current_profile, m_conf->isDebug());
}

void MainWindow::showNotification(const QString &msg)
{
#ifdef Q_OS_LINUX
    //Using DBus to send message.
    QDBusMessage method = QDBusMessage::createMethodCall("org.freedesktop.Notifications","/org/freedesktop/Notifications", "org.freedesktop.Notifications", "Notify");
    QVariantList args;
    args << QCoreApplication::applicationName() << quint32(0) << "shadowsocks-qt5" << "Shadowsocks Qt5" << msg << QStringList () << QVariantMap() << qint32(-1);
    method.setArguments(args);
    QDBusConnection::sessionBus().asyncCall(method);
#else
    systray.showMessage("Shadowsocks Qt5", msg);
#endif
}

void MainWindow::deleteProfile()
{
    int i = ui->profileComboBox->currentIndex();
    m_conf->deleteProfile(i);
    ui->profileComboBox->removeItem(i);
}

void MainWindow::onProcessStarted()
{
    systrayMenu.actions().at(1)->setEnabled(false);
    systrayMenu.actions().at(2)->setEnabled(true);
    ui->stopButton->setEnabled(true);
    ui->startButton->setEnabled(false);
    ui->logBrowser->clear();

    systray.setIcon(QIcon(":/icon/running_icon.png"));
    showNotification(tr("Profile: %1 Started").arg(current_profile->profileName));
}

void MainWindow::onProcessStopped()
{
    systrayMenu.actions().at(1)->setEnabled(true);
    systrayMenu.actions().at(2)->setEnabled(false);
    ui->stopButton->setEnabled(false);
    ui->startButton->setEnabled(true);

#ifdef Q_OS_WIN
    systray.setIcon(QIcon(":/icon/black_icon.png"));
#else
    systray.setIcon(QIcon(":/icon/mono_icon.png"));
#endif

    showNotification(tr("Profile: %1 Stopped").arg(current_profile->profileName));
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

void MainWindow::onProcessReadyRead(const QByteArray &o)
{
    QString str = QString::fromLocal8Bit(o).trimmed();
    if (verboseOutput) {
        qDebug() << str;
    }

    ui->logBrowser->append(str);
    ui->logBrowser->moveCursor(QTextCursor::End);
}

void MainWindow::onConfigurationChanged(bool saved)
{
    ui->profileEditButtonBox->setEnabled(!saved);
    ui->miscSaveButton->setEnabled(!saved);
}

void MainWindow::onBackendTypeChanged(const QString &type)
{
    current_profile->type = type;

    ui->backendEdit->setText(current_profile->getBackend(m_conf->isRelativePath()));

    SSProfile::BackendType tID = current_profile->getBackendType();
    if (tID == SSProfile::GO) {//shadowsocks-go doesn't support timeout argument
        ui->timeoutSpinBox->setEnabled(false);
        ui->timeoutLabel->setEnabled(false);
    }
    else {
        ui->timeoutSpinBox->setEnabled(true);
        ui->timeoutLabel->setEnabled(true);
    }
    if (tID == SSProfile::LIBQSS) {//libQtShadowsocks is statically or dynamically linked
        ui->backendEdit->setEnabled(false);
        ui->backendLabel->setEnabled(false);
        ui->backendToolButton->setEnabled(false);
        ui->customArgEdit->setEnabled(false);
        ui->customArgLabel->setEnabled(false);
    }
    else {
        ui->backendEdit->setEnabled(true);
        ui->backendLabel->setEnabled(true);
        ui->backendToolButton->setEnabled(true);
        ui->customArgEdit->setEnabled(true);
        ui->customArgLabel->setEnabled(true);
    }

#ifdef Q_OS_LINUX
    if ((tID == SSProfile::LIBEV || tID == SSProfile::PYTHON) && m_conf->isTFOAvailable()) {
        ui->tfoCheckBox->setEnabled(true);
    }
    else {
        ui->tfoCheckBox->setEnabled(false);
    }
#endif
    emit configurationChanged();
}

void MainWindow::onServerEditFinished(const QString &str)
{
    current_profile->server = str;
    emit configurationChanged();
}

void MainWindow::onSPortEditFinished(const QString &str)
{
    current_profile->server_port = str;
    emit configurationChanged();
}

void MainWindow::onPasswordEditFinished(const QString &str)
{
    current_profile->password = str;
    emit configurationChanged();
}

void MainWindow::onLAddrEditFinished(const QString &str)
{
    current_profile->local_addr = str;
    emit configurationChanged();
}

void MainWindow::onLPortEditFinished(const QString &str)
{
    current_profile->local_port = str;
    emit configurationChanged();
}

void MainWindow::onMethodChanged(const QString &m)
{
    current_profile->method = m;
    emit configurationChanged();
}

void MainWindow::onTimeoutChanged(int t)
{
    current_profile->timeout = QString::number(t);
    emit configurationChanged();
}

#ifdef Q_OS_LINUX
void MainWindow::onTcpFastOpenChanged(bool t)
{
    current_profile->fast_open = t;
    emit configurationChanged();
}
#endif

void MainWindow::onAutoHideToggled(bool c)
{
    m_conf->setAutoHide(c);
    emit configurationChanged();
}

void MainWindow::onAutoStartToggled(bool c)
{
    m_conf->setAutoStart(c);
    emit configurationChanged();
}

void MainWindow::onDebugToggled(bool c)
{
    m_conf->setDebug(c);
    emit configurationChanged();
}

void MainWindow::onTransculentToggled(bool c)
{
    m_conf->setTranslucent(c);
    emit configurationChanged();
}

void MainWindow::onRelativePathToggled(bool r)
{
    m_conf->setRelativePath(r);
    emit configurationChanged();
}

void MainWindow::blockChildrenSignals(bool b)
{
    QList<QWidget *> children = this->findChildren<QWidget *>();
    for (QList<QWidget *>::iterator it = children.begin(); it != children.end(); ++it) {
        (*it)->blockSignals(b);
    }
}
