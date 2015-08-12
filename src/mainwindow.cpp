#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "connection.h"
#include "editdialog.h"
#include "urihelper.h"
#include "uriinputdialog.h"
#include "sharedialog.h"
#include "logdialog.h"
#include "statusdialog.h"
#include "settingsdialog.h"

#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QScreen>
#include <QCloseEvent>
#include <botan/version.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //setup Settings menu
    ui->menuSettings->addAction(ui->toolBar->toggleViewAction());
    ui->menuSettings->addSeparator();
    ui->menuSettings->addAction(ui->actionGeneralSettings);

    //initialisation
    configHelper = new ConfigHelper(this);
    ui->connectionView->setModel(configHelper->getModel());
    ui->connectionView->resizeColumnsToContents();
    ui->toolBar->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(configHelper->getToolbarStyle()));

    notifier = new StatusNotifier(this, this);

    connect(configHelper, &ConfigHelper::toolbarStyleChanged, ui->toolBar, &QToolBar::setToolButtonStyle);
    connect(configHelper, &ConfigHelper::rowStatusChanged, this, &MainWindow::onConnectionStatusChanged);
    connect(configHelper, &ConfigHelper::connectionStartFailed, [this] {
        QMessageBox::critical(this, tr("Connect Failed"), tr("Local address or port may be invalid or already in use."));
    });
    connect(configHelper, &ConfigHelper::message, notifier, &StatusNotifier::showNotification);
    connect(ui->actionSaveManually, &QAction::triggered, configHelper, &ConfigHelper::save);
    connect(ui->actionTestAllLatency, &QAction::triggered, configHelper, &ConfigHelper::testAllLatency);

    //Move to the center of the screen
    this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());

    //UI signals
    connect(ui->actionImportGUIJson, &QAction::triggered, this, &MainWindow::onImportGuiJson);
    connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionManually, &QAction::triggered, this, &MainWindow::onAddManually);
    connect(ui->actionQRCode, &QAction::triggered, this, &MainWindow::onAddScreenQRCode);
    connect(ui->actionQRCodeFromFile, &QAction::triggered, this, &MainWindow::onAddQRCodeFile);
    connect(ui->actionURI, &QAction::triggered, this, &MainWindow::onAddFromURI);
    connect(ui->actionFromConfigJson, &QAction::triggered, this, &MainWindow::onAddFromConfigJSON);
    connect(ui->actionDelete, &QAction::triggered, this, &MainWindow::onDelete);
    connect(ui->actionEdit, &QAction::triggered, this, &MainWindow::onEdit);
    connect(ui->actionShare, &QAction::triggered, this, &MainWindow::onShare);
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::onConnect);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::onDisconnect);
    connect(ui->actionTestLatency, &QAction::triggered, this, &MainWindow::onLatencyTest);
    connect(ui->actionViewLog, &QAction::triggered, this, &MainWindow::onViewLog);
    connect(ui->actionStatus, &QAction::triggered, this, &MainWindow::onStatus);
    connect(ui->actionMoveUp, &QAction::triggered, this, &MainWindow::onMoveUp);
    connect(ui->actionMoveDown, &QAction::triggered, this, &MainWindow::onMoveDown);
    connect(ui->actionGeneralSettings, &QAction::triggered, this, &MainWindow::onGeneralSettings);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);
    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionReportBug, &QAction::triggered, this, &MainWindow::onReportBug);

    connect(ui->connectionView, &QTableView::clicked, this, &MainWindow::checkCurrentIndex);
    connect(ui->connectionView, &QTableView::activated, this, &MainWindow::checkCurrentIndex);
    connect(ui->connectionView, &QTableView::doubleClicked, this, &MainWindow::onDoubleClicked);

    /* set custom context menu */
    ui->connectionView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->connectionView, &QTableView::customContextMenuRequested, this, &MainWindow::onCustomContextMenuRequested);

    checkCurrentIndex(ui->connectionView->currentIndex());
}

MainWindow::~MainWindow()
{
    delete ui;
}

const QUrl MainWindow::issueUrl = QUrl("https://github.com/librehat/shadowsocks-qt5/issues");

bool MainWindow::isOnlyOneInstance() const
{
    return configHelper->isOnlyOneInstance();
}

bool MainWindow::isHideWindowOnStartup() const
{
    return configHelper->isHideWindowOnStartup();
}

void MainWindow::startAutoStartConnections()
{
    configHelper->startAllAutoStart();
}

void MainWindow::onImportGuiJson()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Import Connections from gui-config.json"), QString(), "GUI Configuration (gui-config.json)");
    if (!file.isNull()) {
        configHelper->importGuiConfigJson(file);
    }
}

void MainWindow::onAddManually()
{
    Connection *newCon = new Connection;
    newProfile(newCon);
}

void MainWindow::onAddScreenQRCode()
{
    QString uri;
    QList<QScreen *> screens = qApp->screens();
    for (QList<QScreen *>::iterator sc = screens.begin(); sc != screens.end(); ++sc) {
        QImage raw_sc = (*sc)->grabWindow(qApp->desktop()->winId()).toImage();
        QString result = URIHelper::decodeImage(raw_sc);
        if (!result.isNull()) {
            uri = result;
        }
    }
    if (uri.isNull()) {
        QMessageBox::critical(this, tr("QR Code Not Found"), tr("Can't find any QR code image that contains valid URI on your screen(s)."));
    } else {
        Connection *newCon = new Connection(uri, this);
        newProfile(newCon);
    }
}

void MainWindow::onAddQRCodeFile()
{
    QString qrFile = QFileDialog::getOpenFileName(this, tr("Open QR Code Image File"), QString(), "Images (*.png *jpg *jpeg *xpm)");
    if (!qrFile.isNull()) {
        QImage img(qrFile);
        QString uri = URIHelper::decodeImage(img);
        if (uri.isNull()) {
            QMessageBox::critical(this, tr("QR Code Not Found"), tr("Can't find any QR code image that contains valid URI on your screen(s)."));
        } else {
            Connection *newCon = new Connection(uri, this);
            newProfile(newCon);
        }
    }
}

void MainWindow::onAddFromURI()
{
    URIInputDialog *inputDlg = new URIInputDialog(this);
    connect(inputDlg, &URIInputDialog::finished, inputDlg, &URIInputDialog::deleteLater);
    connect(inputDlg, &URIInputDialog::acceptedURI, [&](const QString &uri){
        Connection *newCon = new Connection(uri, this);
        newProfile(newCon);
    });
    inputDlg->exec();
}

void MainWindow::onAddFromConfigJSON()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open config.json"), QString(), "JSON (*.json)");
    if (!file.isNull()) {
        Connection *con = configHelper->configJsonToConnection(file);
        if (con) {
            newProfile(con);
        }
    }
}

void MainWindow::onDelete()
{
    configHelper->deleteRow(ui->connectionView->currentIndex().row());
    checkCurrentIndex(ui->connectionView->currentIndex());
}

void MainWindow::onEdit()
{
    editRow(ui->connectionView->currentIndex().row());
}

void MainWindow::onDoubleClicked(const QModelIndex &index)
{
    int row = index.row();
    if (configHelper->connectionAt(row)->isRunning()) {
        onStatus();
    } else {
        editRow(row);
    }
}

void MainWindow::onShare()
{
    QByteArray uri = configHelper->connectionAt(ui->connectionView->currentIndex().row())->getURI();
    ShareDialog *shareDlg = new ShareDialog(uri, this);
    connect(shareDlg, &ShareDialog::finished, shareDlg, &ShareDialog::deleteLater);
    shareDlg->exec();
}

void MainWindow::onConnect()
{
    int row = ui->connectionView->currentIndex().row();
    Connection *con = configHelper->connectionAt(row);
    if (con->isValid()) {
        con->start();
        configHelper->updateTimeAtRow(row);
    } else {
        QMessageBox::critical(this, tr("Invalid"), tr("The connection's profile is invalid!"));
    }
}

void MainWindow::onDisconnect()
{
    int row = ui->connectionView->currentIndex().row();
    configHelper->connectionAt(row)->stop();
}

void MainWindow::onConnectionStatusChanged(const int row, const bool running)
{
    if (ui->connectionView->currentIndex().row() == row) {
        ui->actionConnect->setEnabled(!running);
        ui->actionDisconnect->setEnabled(running);
    }
}

void MainWindow::onLatencyTest()
{
    configHelper->latencyTestAtRow(ui->connectionView->currentIndex().row());
}

void MainWindow::onViewLog()
{
    Connection *con = configHelper->connectionAt(ui->connectionView->currentIndex().row());
    LogDialog *logDlg = new LogDialog(con, this);
    connect(logDlg, &LogDialog::finished, logDlg, &LogDialog::deleteLater);
    logDlg->exec();
}

void MainWindow::onStatus()
{
    Connection *con = configHelper->connectionAt(ui->connectionView->currentIndex().row());
    StatusDialog *statusDlg = new StatusDialog(con, this);
    connect(statusDlg, &StatusDialog::finished, statusDlg, &StatusDialog::deleteLater);
    statusDlg->exec();
}

void MainWindow::onMoveUp()
{
    QModelIndex index = configHelper->moveUp(ui->connectionView->currentIndex().row());
    ui->connectionView->setCurrentIndex(index);
    checkCurrentIndex(index);
}

void MainWindow::onMoveDown()
{
    QModelIndex index = configHelper->moveDown(ui->connectionView->currentIndex().row());
    ui->connectionView->setCurrentIndex(index);
    checkCurrentIndex(index);
}

void MainWindow::onGeneralSettings()
{
    SettingsDialog *sDlg = new SettingsDialog(configHelper, this);
    connect(sDlg, &SettingsDialog::finished, sDlg, &SettingsDialog::deleteLater);
    sDlg->exec();
}

void MainWindow::newProfile(Connection *newCon)
{
    EditDialog *editDlg = new EditDialog(newCon, this);
    connect(editDlg, &EditDialog::finished, editDlg, &EditDialog::deleteLater);
    if (editDlg->exec()) {//accepted
        configHelper->addConnection(newCon);
    } else {
        newCon->deleteLater();
    }
}

void MainWindow::editRow(int row)
{
    Connection *con = configHelper->connectionAt(row);
    EditDialog *editDlg = new EditDialog(con, this);
    connect(editDlg, &EditDialog::finished, editDlg, &EditDialog::deleteLater);
    if (editDlg->exec()) {
        configHelper->updateNameAtRow(row);
    }
}

void MainWindow::checkCurrentIndex(const QModelIndex &index)
{
    const bool valid = index.isValid();
    ui->actionConnect->setEnabled(valid);
    ui->actionDisconnect->setEnabled(valid);
    ui->actionTestLatency->setEnabled(valid);
    ui->actionEdit->setEnabled(valid);
    ui->actionDelete->setEnabled(valid);
    ui->actionShare->setEnabled(valid);
    ui->actionViewLog->setEnabled(valid);
    ui->actionStatus->setEnabled(valid);
    ui->actionMoveUp->setEnabled(valid ? index.row() > 0 : false);
    ui->actionMoveDown->setEnabled(valid ? index.row() < configHelper->size() - 1 : false);

    if (valid) {
        const bool &running = configHelper->connectionAt(index.row())->isRunning();
        ui->actionConnect->setEnabled(!running);
        ui->actionDisconnect->setEnabled(running);
    }
}

void MainWindow::onAbout()
{
    QString text = QString("<h1>Shadowsocks-Qt5</h1><p><b>Version %1</b><br />Using libQtShadowsocks %2<br />Using Botan %3.%4.%5</p><p>Copyright © 2014-2015 Symeon Huang (<a href='https://twitter.com/librehat'>@librehat</a>)</p><p>License: <a href='http://www.gnu.org/licenses/lgpl.html'>GNU Lesser General Public License Version 3</a><br />Project Hosted at <a href='https://github.com/librehat/shadowsocks-qt5'>GitHub</a></p>").arg(QStringLiteral(APP_VERSION)).arg(QSS::Common::version().data()).arg(Botan::version_major()).arg(Botan::version_minor()).arg(Botan::version_patch());
    QMessageBox::about(this, tr("About"), text);
}

void MainWindow::onReportBug()
{
    QDesktopServices::openUrl(issueUrl);
}

void MainWindow::onCustomContextMenuRequested(const QPoint &pos)
{
    this->checkCurrentIndex(ui->connectionView->indexAt(pos));
    ui->menuConnection->popup(ui->connectionView->viewport()->mapToGlobal(pos));
}

void MainWindow::hideEvent(QHideEvent *e)
{
    QMainWindow::hideEvent(e);
    notifier->onWindowVisibleChanged(false);
}

void MainWindow::showEvent(QShowEvent *e)
{
    QMainWindow::showEvent(e);
    notifier->onWindowVisibleChanged(true);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
#ifdef Q_OS_UNIX
    QMainWindow::closeEvent(e);
#else //but Windows will quit this application, so we have to ignore the event
    e->ignore();
    hide();
#endif
}
