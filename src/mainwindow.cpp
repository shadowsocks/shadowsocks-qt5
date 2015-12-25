#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "connection.h"
#include "editdialog.h"
#include "urihelper.h"
#include "uriinputdialog.h"
#include "sharedialog.h"
#include "logdialog.h"
#include "settingsdialog.h"
#include "qrcodecapturer.h"

#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <botan/version.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //setup Settings menu
    ui->menuSettings->addAction(ui->toolBar->toggleViewAction());

    //initialisation
    model = new ConnectionTableModel(this);
    configHelper = new ConfigHelper(model, this);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setSortRole(Qt::EditRole);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);//read from all columns
    ui->connectionView->setModel(proxyModel);
    ui->connectionView->resizeColumnsToContents();
    ui->toolBar->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(configHelper->getToolbarStyle()));

    notifier = new StatusNotifier(this, this->isHideWindowOnStartup(), this);

    connect(configHelper, &ConfigHelper::toolbarStyleChanged, ui->toolBar, &QToolBar::setToolButtonStyle);
    connect(model, &ConnectionTableModel::message, notifier, &StatusNotifier::showNotification);
    connect(model, &ConnectionTableModel::rowStatusChanged, this, &MainWindow::onConnectionStatusChanged);
    connect(ui->actionSaveManually, &QAction::triggered, configHelper, &ConfigHelper::save);
    connect(ui->actionTestAllLatency, &QAction::triggered, model, &ConnectionTableModel::testAllLatency);

    //some UI changes accoding to config
    ui->toolBar->setVisible(configHelper->isShowToolbar());
    ui->actionShowFilterBar->setChecked(configHelper->isShowFilterBar());
    ui->menuBar->setNativeMenuBar(configHelper->isNativeMenuBar());

    //Move to the center of the screen
    this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());

    //UI signals
    connect(ui->actionImportGUIJson, &QAction::triggered, this, &MainWindow::onImportGuiJson);
    connect(ui->actionExportGUIJson, &QAction::triggered, this, &MainWindow::onExportGuiJson);
    connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionManually, &QAction::triggered, this, &MainWindow::onAddManually);
    connect(ui->actionQRCode, &QAction::triggered, this, &MainWindow::onAddScreenQRCode);
    connect(ui->actionScanQRCodeCapturer, &QAction::triggered, this, &MainWindow::onAddScreenQRCodeCapturer);
    connect(ui->actionQRCodeFromFile, &QAction::triggered, this, &MainWindow::onAddQRCodeFile);
    connect(ui->actionURI, &QAction::triggered, this, &MainWindow::onAddFromURI);
    connect(ui->actionFromConfigJson, &QAction::triggered, this, &MainWindow::onAddFromConfigJSON);
    connect(ui->actionDelete, &QAction::triggered, this, &MainWindow::onDelete);
    connect(ui->actionEdit, &QAction::triggered, this, &MainWindow::onEdit);
    connect(ui->actionShare, &QAction::triggered, this, &MainWindow::onShare);
    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::onConnect);
    connect(ui->actionForceConnect, &QAction::triggered, this, &MainWindow::onForceConnect);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::onDisconnect);
    connect(ui->actionTestLatency, &QAction::triggered, this, &MainWindow::onLatencyTest);
    connect(ui->actionViewLog, &QAction::triggered, this, &MainWindow::onViewLog);
    connect(ui->actionMoveUp, &QAction::triggered, this, &MainWindow::onMoveUp);
    connect(ui->actionMoveDown, &QAction::triggered, this, &MainWindow::onMoveDown);
    connect(ui->actionGeneralSettings, &QAction::triggered, this, &MainWindow::onGeneralSettings);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);
    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionReportBug, &QAction::triggered, this, &MainWindow::onReportBug);
    connect(ui->actionShowFilterBar, &QAction::toggled, configHelper, &ConfigHelper::setShowFilterBar);
    connect(ui->actionShowFilterBar, &QAction::toggled, this, &MainWindow::onFilterToggled);
    connect(ui->toolBar, &QToolBar::visibilityChanged, configHelper, &ConfigHelper::setShowToolbar);
    connect(ui->filterLineEdit, &QLineEdit::textChanged, this, &MainWindow::onFilterTextChanged);

    connect(ui->connectionView, &QTableView::clicked, this, static_cast<void (MainWindow::*)(const QModelIndex&)>(&MainWindow::checkCurrentIndex));
    connect(ui->connectionView, &QTableView::activated, this, static_cast<void (MainWindow::*)(const QModelIndex&)>(&MainWindow::checkCurrentIndex));
    connect(ui->connectionView, &QTableView::doubleClicked, this, &MainWindow::onEdit);

    /* set custom context menu */
    ui->connectionView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->connectionView, &QTableView::customContextMenuRequested, this, &MainWindow::onCustomContextMenuRequested);

    checkCurrentIndex();
}

MainWindow::~MainWindow()
{
    delete ui;
    configHelper->save();
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

void MainWindow::onExportGuiJson()
{
    QString file = QFileDialog::getSaveFileName(this, tr("Export Connections as gui-config.json"), QString("gui-config.json"), "GUI Configuration (gui-config.json)");
    if (!file.isNull()) {
        configHelper->exportGuiConfigJson(file);
    }
}

void MainWindow::onAddManually()
{
    Connection *newCon = new Connection;
    newProfile(newCon);
}

void MainWindow::onAddScreenQRCode()
{
    QString uri = QRCodeCapturer::scanEntireScreen();
    if (uri.isNull()) {
        QMessageBox::critical(this, tr("QR Code Not Found"), tr("Can't find any QR code image that contains valid URI on your screen(s)."));
    } else {
        Connection *newCon = new Connection(uri, this);
        newProfile(newCon);
    }
}

void MainWindow::onAddScreenQRCodeCapturer()
{
    QRCodeCapturer *capturer = new QRCodeCapturer(this);
    connect(capturer, &QRCodeCapturer::finished, capturer, &QRCodeCapturer::deleteLater);
    connect(capturer, &QRCodeCapturer::qrCodeFound, [this](const QString &uri){
        Connection *newCon = new Connection(uri, this);
        newProfile(newCon);
    });
    capturer->exec();
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
    model->removeRow(proxyModel->mapToSource(ui->connectionView->currentIndex()).row());
    checkCurrentIndex();
}

void MainWindow::onEdit()
{
    editRow(proxyModel->mapToSource(ui->connectionView->currentIndex()).row());
}

void MainWindow::onShare()
{
    QByteArray uri = model->getItem(proxyModel->mapToSource(ui->connectionView->currentIndex()).row())->getConnection()->getURI();
    ShareDialog *shareDlg = new ShareDialog(uri, this);
    connect(shareDlg, &ShareDialog::finished, shareDlg, &ShareDialog::deleteLater);
    shareDlg->exec();
}

void MainWindow::onConnect()
{
    int row = proxyModel->mapToSource(ui->connectionView->currentIndex()).row();
    Connection *con = model->getItem(row)->getConnection();
    if (con->isValid()) {
        con->start();
    } else {
        QMessageBox::critical(this, tr("Invalid"), tr("The connection's profile is invalid!"));
    }
}

void MainWindow::onForceConnect()
{
    int row = proxyModel->mapToSource(ui->connectionView->currentIndex()).row();
    Connection *con = model->getItem(row)->getConnection();
    if (con->isValid()) {
        model->disconnectConnectionsAt(con->getProfile().localAddress, con->getProfile().localPort);
        con->start();
    } else {
        QMessageBox::critical(this, tr("Invalid"), tr("The connection's profile is invalid!"));
    }
}

void MainWindow::onDisconnect()
{
    int row = proxyModel->mapToSource(ui->connectionView->currentIndex()).row();
    model->getItem(row)->getConnection()->stop();
}

void MainWindow::onConnectionStatusChanged(const int row, const bool running)
{
    if (proxyModel->mapToSource(ui->connectionView->currentIndex()).row() == row) {
        ui->actionConnect->setEnabled(!running);
        ui->actionDisconnect->setEnabled(running);
    }
}

void MainWindow::onLatencyTest()
{
    model->getItem(proxyModel->mapToSource(ui->connectionView->currentIndex()).row())->testLatency();
}

void MainWindow::onViewLog()
{
    Connection *con = model->getItem(proxyModel->mapToSource(ui->connectionView->currentIndex()).row())->getConnection();
    LogDialog *logDlg = new LogDialog(con, this);
    connect(logDlg, &LogDialog::finished, logDlg, &LogDialog::deleteLater);
    logDlg->exec();
}

void MainWindow::onMoveUp()
{
    QModelIndex proxyIndex = ui->connectionView->currentIndex();
    int currentRow = proxyModel->mapToSource(proxyIndex).row();
    int targetRow = proxyModel->mapToSource(proxyModel->index(proxyIndex.row() - 1, proxyIndex.column(), proxyIndex.parent())).row();
    model->move(currentRow, targetRow);
    checkCurrentIndex();
}

void MainWindow::onMoveDown()
{
    QModelIndex proxyIndex = ui->connectionView->currentIndex();
    int currentRow = proxyModel->mapToSource(proxyIndex).row();
    int targetRow = proxyModel->mapToSource(proxyModel->index(proxyIndex.row() + 1, proxyIndex.column(), proxyIndex.parent())).row();
    model->move(currentRow, targetRow);
    checkCurrentIndex();
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
        model->appendConnection(newCon);
    } else {
        newCon->deleteLater();
    }
}

void MainWindow::editRow(int row)
{
    Connection *con = model->getItem(row)->getConnection();
    EditDialog *editDlg = new EditDialog(con, this);
    connect(editDlg, &EditDialog::finished, editDlg, &EditDialog::deleteLater);
    editDlg->exec();
}

void MainWindow::checkCurrentIndex()
{
    checkCurrentIndex(ui->connectionView->currentIndex());
}

void MainWindow::checkCurrentIndex(const QModelIndex &_index)
{
    QModelIndex index = proxyModel->mapToSource(_index);
    const bool valid = index.isValid();
    ui->actionTestLatency->setEnabled(valid);
    ui->actionEdit->setEnabled(valid);
    ui->actionDelete->setEnabled(valid);
    ui->actionShare->setEnabled(valid);
    ui->actionViewLog->setEnabled(valid);
    ui->actionMoveUp->setEnabled(valid ? _index.row() > 0 : false);
    ui->actionMoveDown->setEnabled(valid ? _index.row() < model->rowCount() - 1 : false);

    if (valid) {
        const bool &running = model->getItem(index.row())->getConnection()->isRunning();
        ui->actionConnect->setEnabled(!running);
        ui->actionForceConnect->setEnabled(!running);
        ui->actionDisconnect->setEnabled(running);
    } else {
        ui->actionConnect->setEnabled(false);
        ui->actionForceConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(false);
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

void MainWindow::onFilterToggled(bool show)
{
    if (show) {
        ui->filterLineEdit->setFocus();
    }
}

void MainWindow::onFilterTextChanged(const QString &text)
{
    proxyModel->setFilterWildcard(text);
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
    this->setFocus();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    if (e->spontaneous()) {
        e->ignore();
        hide();
    } else {
        QMainWindow::closeEvent(e);
    }
}
