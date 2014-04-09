#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //SIGNALs and SLOTs
    connect(ui->backendToolButton, &QToolButton::clicked, this, &MainWindow::getSSLocalPath);
    connect(ui->profileComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(oncurrentProfileChanged(int)));
    connect(ui->addProfileButton, &QToolButton::clicked, this, &MainWindow::addProfileDialogue);
    connect(ui->apply_abort, &QDialogButtonBox::rejected, this, &MainWindow::apply_abort_Rejected);
    connect(ui->apply_abort, &QDialogButtonBox::accepted, this, &MainWindow::apply_abort_Accepted);
    connect(ui->profileEditButtonBox, &QDialogButtonBox::clicked, this, &MainWindow::profileEditButtonClicked);
    connect(ui->delProfileButton, &QToolButton::clicked, this, &MainWindow::deleteProfile);
    connect(this, &MainWindow::currentProfileChanged, this, &MainWindow::oncurrentProfileChanged);
    connect(&ss_local, &SS_Process::readReadyProcess, this, &MainWindow::onreadReadyProcess);
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
    ui->backendEdit->setText(detectSSLocal());
    ui->profileComboBox->insertItems(0, m_profile->getserverList());
    ui->profileComboBox->setCurrentIndex(m_profile->getIndex());

    //desktop systray
    systrayMenu.addAction("Show/Hide", this, SLOT(showorhideWindow()));
    systrayMenu.addAction("Start", this, SLOT(apply_abort_Accepted()));
    systrayMenu.addAction("Stop", this, SLOT(apply_abort_Rejected()));
    systrayMenu.addAction("Exit", this, SLOT(close()));
    systrayMenu.actions().at(1)->setEnabled(false);
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
}

MainWindow::~MainWindow()
{
    ss_local.stop();//prevent crashes
    delete ui;
    delete m_profile;
}

void MainWindow::getSSLocalPath()
{
    ui->backendEdit->setText(QFileDialog::getOpenFileName());
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

    ui->profileComboBox->setCurrentText(current_profile.server);
    ui->sportEdit->setText(current_profile.server_port);
    ui->pwdEdit->setText(current_profile.password);
    ui->lportEdit->setText(current_profile.local_port);
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
        ui->profileComboBox->insertItem(ui->profileComboBox->count(), server);

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
    if (m_profile->app.isEmpty()) {
    #ifdef _WIN32
        QString bundled_sslocal = QCoreApplication::applicationDirPath() + "/ss-local.exe";
        if(QFile::exists(bundled_sslocal)) {
            return QDir::toNativeSeparators(bundled_sslocal);
        }
        else
            return QString("");
    #endif
        return QStandardPaths::findExecutable("ss-local");
    }
    else
        return QDir::toNativeSeparators(m_profile->app);
}

void MainWindow::profileEditButtonClicked(QAbstractButton *b)
{
    if (ui->profileEditButtonBox->standardButton(b) == QDialogButtonBox::Save) {
        current_profile.server = ui->profileComboBox->currentText();
        current_profile.server_port = ui->sportEdit->text();
        current_profile.password = ui->pwdEdit->text();
        current_profile.local_port = ui->lportEdit->text();
        current_profile.method = ui->methodComboBox->currentText();
        current_profile.timeout = ui->timeoutSpinBox->cleanText();
        m_profile->app = ui->backendEdit->text();
        m_profile->saveProfile(ui->profileComboBox->currentIndex(), current_profile);
        m_profile->saveProfileToJSON();
    }
    else {//reset
        ss_local.stop();
        m_profile->revert();
        ui->backendEdit->setText(detectSSLocal());
        disconnect(ui->profileComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(oncurrentProfileChanged(int)));
        ui->profileComboBox->clear();
        connect(ui->profileComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(oncurrentProfileChanged(int)));
        ui->profileComboBox->insertItems(0, m_profile->getserverList());
        ui->profileComboBox->setCurrentIndex(m_profile->getIndex());
    }
}

void MainWindow::apply_abort_Accepted()
{
    ss_local.setapp(ui->backendEdit->text());
    ss_local.start(current_profile);
}

void MainWindow::apply_abort_Rejected()
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
    ui->apply_abort->button(QDialogButtonBox::Abort)->setEnabled(true);
    ui->apply_abort->button(QDialogButtonBox::Apply)->setEnabled(false);

    systray.setIcon(QIcon(":/icon/running_icon.png"));
}

void MainWindow::processStopped()
{
    systrayMenu.actions().at(1)->setEnabled(true);
    systrayMenu.actions().at(2)->setEnabled(false);
    ui->apply_abort->button(QDialogButtonBox::Abort)->setEnabled(false);
    ui->apply_abort->button(QDialogButtonBox::Apply)->setEnabled(true);

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
        ui->apply_abort->setFocus();
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

void MainWindow::onreadReadyProcess(const QByteArray &o)
{
    ui->logBrowser->moveCursor(QTextCursor::End);
    ui->logBrowser->append(o);
    ui->logBrowser->moveCursor(QTextCursor::End);
}
