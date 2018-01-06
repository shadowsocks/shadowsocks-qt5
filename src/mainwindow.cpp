#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "connection.h"
#include "editdialog.h"
#include "urihelper.h"
#include "uriinputdialog.h"
#include "sharedialog.h"
#include "settingsdialog.h"
#include "qrcodecapturer.h"

#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QLocalSocket>

MainWindow::MainWindow(ConfigHelper *confHelper, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    configHelper(confHelper),
    instanceRunning(false)
{
    Q_ASSERT(configHelper);

    initSingleInstance();

    ui->setupUi(this);

    //setup Settings menu
#ifndef Q_OS_DARWIN
    ui->menuSettings->addAction(ui->toolBar->toggleViewAction());
#endif

    //initialisation
    model = new ConnectionTableModel(this);
    configHelper->read(model);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setSortRole(Qt::EditRole);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1);//read from all columns
    ui->connectionView->setModel(proxyModel);
    ui->toolBar->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>
                                    (configHelper->getToolbarStyle()));
    setupActionIcon();

    notifier = new StatusNotifier(this, configHelper->isHideWindowOnStartup(), this);

    connect(configHelper, &ConfigHelper::toolbarStyleChanged,
            ui->toolBar, &QToolBar::setToolButtonStyle);
    connect(model, &ConnectionTableModel::message,
            notifier, &StatusNotifier::showNotification);
    connect(model, &ConnectionTableModel::rowStatusChanged,
            this, &MainWindow::onConnectionStatusChanged);
    connect(ui->actionSaveManually, &QAction::triggered,
            this, &MainWindow::onSaveManually);
    connect(ui->actionTestAllLatency, &QAction::triggered,
            model, &ConnectionTableModel::testAllLatency);

    //some UI changes accoding to config
    ui->toolBar->setVisible(configHelper->isShowToolbar());
    ui->actionShowFilterBar->setChecked(configHelper->isShowFilterBar());
    ui->menuBar->setNativeMenuBar(configHelper->isNativeMenuBar());

    //Move to the center of the screen
    this->move(QApplication::desktop()->screen()->rect().center() -
               this->rect().center());

    //UI signals
    connect(ui->actionImportGUIJson, &QAction::triggered,
            this, &MainWindow::onImportGuiJson);
    connect(ui->actionExportGUIJson, &QAction::triggered,
            this, &MainWindow::onExportGuiJson);
    connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionManually, &QAction::triggered,
            this, &MainWindow::onAddManually);
    connect(ui->actionQRCode, &QAction::triggered,
            this, &MainWindow::onAddScreenQRCode);
    connect(ui->actionScanQRCodeCapturer, &QAction::triggered,
            this, &MainWindow::onAddScreenQRCodeCapturer);
    connect(ui->actionQRCodeFromFile, &QAction::triggered,
            this, &MainWindow::onAddQRCodeFile);
    connect(ui->actionURI, &QAction::triggered,
            this, &MainWindow::onAddFromURI);
    connect(ui->actionFromConfigJson, &QAction::triggered,
            this, &MainWindow::onAddFromConfigJSON);
    connect(ui->actionDelete, &QAction::triggered, this, &MainWindow::onDelete);
    connect(ui->actionEdit, &QAction::triggered, this, &MainWindow::onEdit);
    connect(ui->actionShare, &QAction::triggered, this, &MainWindow::onShare);
    connect(ui->actionConnect, &QAction::triggered,
            this, &MainWindow::onConnect);
    connect(ui->actionForceConnect, &QAction::triggered,
            this, &MainWindow::onForceConnect);
    connect(ui->actionDisconnect, &QAction::triggered,
            this, &MainWindow::onDisconnect);
    connect(ui->actionTestLatency, &QAction::triggered,
            this, &MainWindow::onLatencyTest);
    connect(ui->actionMoveUp, &QAction::triggered, this, &MainWindow::onMoveUp);
    connect(ui->actionMoveDown, &QAction::triggered,
            this, &MainWindow::onMoveDown);
    connect(ui->actionGeneralSettings, &QAction::triggered,
            this, &MainWindow::onGeneralSettings);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);
    connect(ui->actionAboutQt, &QAction::triggered,
            qApp, &QApplication::aboutQt);
    connect(ui->actionReportBug, &QAction::triggered,
            this, &MainWindow::onReportBug);
    connect(ui->actionShowFilterBar, &QAction::toggled,
            configHelper, &ConfigHelper::setShowFilterBar);
    connect(ui->actionShowFilterBar, &QAction::toggled,
            this, &MainWindow::onFilterToggled);
    connect(ui->toolBar, &QToolBar::visibilityChanged,
            configHelper, &ConfigHelper::setShowToolbar);
    connect(ui->filterLineEdit, &QLineEdit::textChanged,
            this, &MainWindow::onFilterTextChanged);

    connect(ui->connectionView, &QTableView::clicked,
            this, static_cast<void (MainWindow::*)(const QModelIndex&)>
            (&MainWindow::checkCurrentIndex));
    connect(ui->connectionView, &QTableView::activated,
            this, static_cast<void (MainWindow::*)(const QModelIndex&)>
            (&MainWindow::checkCurrentIndex));
    connect(ui->connectionView, &QTableView::doubleClicked,
            this, &MainWindow::onEdit);

    /* set custom context menu */
    ui->connectionView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->connectionView, &QTableView::customContextMenuRequested,
            this, &MainWindow::onCustomContextMenuRequested);

    checkCurrentIndex();

    // Restore mainWindow's geometry and state
    restoreGeometry(configHelper->getMainWindowGeometry());
    restoreState(configHelper->getMainWindowState());
    ui->connectionView->horizontalHeader()->restoreGeometry(configHelper->getTableGeometry());
    ui->connectionView->horizontalHeader()->restoreState(configHelper->getTableState());
}

MainWindow::~MainWindow()
{
    configHelper->save(*model);
    configHelper->setTableGeometry(ui->connectionView->horizontalHeader()->saveGeometry());
    configHelper->setTableState(ui->connectionView->horizontalHeader()->saveState());
    configHelper->setMainWindowGeometry(saveGeometry());
    configHelper->setMainWindowState(saveState());

    // delete ui after everything in case it's deleted while still needed for
    // the functions written above
    delete ui;
}

const QUrl MainWindow::issueUrl =
        QUrl("https://github.com/shadowsocks/shadowsocks-qt5/issues");

void MainWindow::startAutoStartConnections()
{
    configHelper->startAllAutoStart(*model);
}

void MainWindow::onImportGuiJson()
{
    QString file = QFileDialog::getOpenFileName(
                   this,
                   tr("Import Connections from gui-config.json"),
                   QString(),
                   "GUI Configuration (gui-config.json)");
    if (!file.isNull()) {
        configHelper->importGuiConfigJson(model, file);
    }
}

void MainWindow::onExportGuiJson()
{
    QString file = QFileDialog::getSaveFileName(
                   this,
                   tr("Export Connections as gui-config.json"),
                   QString("gui-config.json"),
                   "GUI Configuration (gui-config.json)");
    if (!file.isNull()) {
        configHelper->exportGuiConfigJson(*model, file);
    }
}

void MainWindow::onSaveManually()
{
    configHelper->save(*model);
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
        QMessageBox::critical(
                    this,
                    tr("QR Code Not Found"),
                    tr("Can't find any QR code image that contains "
                       "valid URI on your screen(s)."));
    } else {
        Connection *newCon = new Connection(uri, this);
        newProfile(newCon);
    }
}

void MainWindow::onAddScreenQRCodeCapturer()
{
    QRCodeCapturer *capturer = new QRCodeCapturer(this);
    connect(capturer, &QRCodeCapturer::closed,
            capturer, &QRCodeCapturer::deleteLater);
    connect(capturer, &QRCodeCapturer::qrCodeFound,
            this, &MainWindow::onQRCodeCapturerResultFound,
            Qt::DirectConnection);
    capturer->show();
}

void MainWindow::onAddQRCodeFile()
{
    QString qrFile =
            QFileDialog::getOpenFileName(this,
                                         tr("Open QR Code Image File"),
                                         QString(),
                                         "Images (*.png *jpg *jpeg *xpm)");
    if (!qrFile.isNull()) {
        QImage img(qrFile);
        QString uri = URIHelper::decodeImage(img);
        if (uri.isNull()) {
            QMessageBox::critical(this,
                                  tr("QR Code Not Found"),
                                  tr("Can't find any QR code image that "
                                     "contains valid URI on your screen(s)."));
        } else {
            Connection *newCon = new Connection(uri, this);
            newProfile(newCon);
        }
    }
}

void MainWindow::onAddFromURI()
{
    URIInputDialog *inputDlg = new URIInputDialog(this);
    connect(inputDlg, &URIInputDialog::finished,
            inputDlg, &URIInputDialog::deleteLater);
    connect(inputDlg, &URIInputDialog::acceptedURI, [&](const QString &uri){
        Connection *newCon = new Connection(uri, this);
        newProfile(newCon);
    });
    inputDlg->exec();
}

void MainWindow::onAddFromConfigJSON()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open config.json"),
                                                QString(), "JSON (*.json)");
    if (!file.isNull()) {
        Connection *con = configHelper->configJsonToConnection(file);
        if (con) {
            newProfile(con);
        }
    }
}

void MainWindow::onDelete()
{
    if (model->removeRow(proxyModel->mapToSource(
                         ui->connectionView->currentIndex()).row())) {
        configHelper->save(*model);
    }
    checkCurrentIndex();
}

void MainWindow::onEdit()
{
    editRow(proxyModel->mapToSource(ui->connectionView->currentIndex()).row());
}

void MainWindow::onShare()
{
    QByteArray uri = model->getItem(
                proxyModel->mapToSource(ui->connectionView->currentIndex()).
                row())->getConnection()->getURI();
    ShareDialog *shareDlg = new ShareDialog(uri, this);
    connect(shareDlg, &ShareDialog::finished,
            shareDlg, &ShareDialog::deleteLater);
    shareDlg->exec();
}

void MainWindow::onConnect()
{
    int row = proxyModel->mapToSource(ui->connectionView->currentIndex()).row();
    Connection *con = model->getItem(row)->getConnection();
    if (con->isValid()) {
        con->start();
    } else {
        QMessageBox::critical(this, tr("Invalid"),
                              tr("The connection's profile is invalid!"));
    }
}

void MainWindow::onForceConnect()
{
    int row = proxyModel->mapToSource(ui->connectionView->currentIndex()).row();
    Connection *con = model->getItem(row)->getConnection();
    if (con->isValid()) {
        model->disconnectConnectionsAt(con->getProfile().localAddress,
                                       con->getProfile().localPort);
        con->start();
    } else {
        QMessageBox::critical(this, tr("Invalid"),
                              tr("The connection's profile is invalid!"));
    }
}

void MainWindow::onDisconnect()
{
    int row = proxyModel->mapToSource(ui->connectionView->currentIndex()).row();
    model->getItem(row)->getConnection()->stop();
}

void MainWindow::onConnectionStatusChanged(const int row, const bool running)
{
    if (proxyModel->mapToSource(
                ui->connectionView->currentIndex()).row() == row) {
        ui->actionConnect->setEnabled(!running);
        ui->actionDisconnect->setEnabled(running);
    }
}

void MainWindow::onLatencyTest()
{
    model->getItem(proxyModel->mapToSource(ui->connectionView->currentIndex()).
                   row())->testLatency();
}

void MainWindow::onMoveUp()
{
    QModelIndex proxyIndex = ui->connectionView->currentIndex();
    int currentRow = proxyModel->mapToSource(proxyIndex).row();
    int targetRow = proxyModel->mapToSource(
                proxyModel->index(proxyIndex.row() - 1,
                                  proxyIndex.column(),
                                  proxyIndex.parent())
                                           ).row();
    model->move(currentRow, targetRow);
    checkCurrentIndex();
}

void MainWindow::onMoveDown()
{
    QModelIndex proxyIndex = ui->connectionView->currentIndex();
    int currentRow = proxyModel->mapToSource(proxyIndex).row();
    int targetRow = proxyModel->mapToSource(
                proxyModel->index(proxyIndex.row() + 1,
                                  proxyIndex.column(),
                                  proxyIndex.parent())
                                           ).row();
    model->move(currentRow, targetRow);
    checkCurrentIndex();
}

void MainWindow::onGeneralSettings()
{
    SettingsDialog *sDlg = new SettingsDialog(configHelper, this);
    connect(sDlg, &SettingsDialog::finished,
            sDlg, &SettingsDialog::deleteLater);
    if (sDlg->exec()) {
        configHelper->save(*model);
        configHelper->setStartAtLogin();
    }
}

void MainWindow::newProfile(Connection *newCon)
{
    EditDialog *editDlg = new EditDialog(newCon, this);
    connect(editDlg, &EditDialog::finished, editDlg, &EditDialog::deleteLater);
    if (editDlg->exec()) {//accepted
        model->appendConnection(newCon);
        configHelper->save(*model);
    } else {
        newCon->deleteLater();
    }
}

void MainWindow::editRow(int row)
{
    Connection *con = model->getItem(row)->getConnection();
    EditDialog *editDlg = new EditDialog(con, this);
    connect(editDlg, &EditDialog::finished, editDlg, &EditDialog::deleteLater);
    if (editDlg->exec()) {
        configHelper->save(*model);
    }
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
    ui->actionMoveUp->setEnabled(valid ? _index.row() > 0 : false);
    ui->actionMoveDown->setEnabled(valid ?
                                   _index.row() < model->rowCount() - 1 :
                                   false);

    if (valid) {
        const bool &running =
                model->getItem(index.row())->getConnection()->isRunning();
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
    QString text = QString("<h1>Shadowsocks-Qt5</h1><p><b>Version %1</b><br />"
            "Using libQtShadowsocks %2</p>"
            "<p>Copyright © 2014-2018 Symeon Huang "
            "(<a href='https://twitter.com/librehat'>"
            "@librehat</a>)</p>"
            "<p>License: <a href='http://www.gnu.org/licenses/lgpl.html'>"
            "GNU Lesser General Public License Version 3</a><br />"
            "Project Hosted at "
            "<a href='https://github.com/shadowsocks/shadowsocks-qt5'>"
            "GitHub</a></p>")
            .arg(QStringLiteral(APP_VERSION))
            .arg(QSS::Common::version());
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

void MainWindow::onQRCodeCapturerResultFound(const QString &uri)
{
    QRCodeCapturer* capturer = qobject_cast<QRCodeCapturer*>(sender());
    // Disconnect immediately to avoid duplicate signals
    disconnect(capturer, &QRCodeCapturer::qrCodeFound,
               this, &MainWindow::onQRCodeCapturerResultFound);
    Connection *newCon = new Connection(uri, this);
    newProfile(newCon);
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

void MainWindow::setupActionIcon()
{
    ui->actionConnect->setIcon(QIcon::fromTheme("network-connect",
                               QIcon::fromTheme("network-vpn")));
    ui->actionDisconnect->setIcon(QIcon::fromTheme("network-disconnect",
                                  QIcon::fromTheme("network-offline")));
    ui->actionEdit->setIcon(QIcon::fromTheme("document-edit",
                            QIcon::fromTheme("accessories-text-editor")));
    ui->actionShare->setIcon(QIcon::fromTheme("document-share",
                             QIcon::fromTheme("preferences-system-sharing")));
    ui->actionTestLatency->setIcon(QIcon::fromTheme("flag",
                                   QIcon::fromTheme("starred")));
    ui->actionImportGUIJson->setIcon(QIcon::fromTheme("document-import",
                                     QIcon::fromTheme("insert-text")));
    ui->actionExportGUIJson->setIcon(QIcon::fromTheme("document-export",
                                     QIcon::fromTheme("document-save-as")));
    ui->actionManually->setIcon(QIcon::fromTheme("edit-guides",
                                QIcon::fromTheme("accessories-text-editor")));
    ui->actionURI->setIcon(QIcon::fromTheme("text-field",
                           QIcon::fromTheme("insert-link")));
    ui->actionQRCode->setIcon(QIcon::fromTheme("edit-image-face-recognize",
                              QIcon::fromTheme("insert-image")));
    ui->actionScanQRCodeCapturer->setIcon(ui->actionQRCode->icon());
    ui->actionGeneralSettings->setIcon(QIcon::fromTheme("configure",
                                       QIcon::fromTheme("preferences-desktop")));
    ui->actionReportBug->setIcon(QIcon::fromTheme("tools-report-bug",
                                 QIcon::fromTheme("help-faq")));
}

bool MainWindow::isInstanceRunning() const
{
    return instanceRunning;
}

void MainWindow::initSingleInstance()
{
    const QString serverName = QCoreApplication::applicationName();
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (socket.waitForConnected(500)) {
        instanceRunning = true;
        if (configHelper->isOnlyOneInstance()) {
            qWarning() << "An instance of ss-qt5 is already running";
        }
        QByteArray username = qgetenv("USER");
        if (username.isEmpty()) {
            username = qgetenv("USERNAME");
        }
        socket.write(username);
        socket.waitForBytesWritten();
        return;
    }

    /* Can't connect to server, indicating it's the first instance of the user */
    instanceServer = new QLocalServer(this);
    instanceServer->setSocketOptions(QLocalServer::WorldAccessOption);
    connect(instanceServer, &QLocalServer::newConnection,
            this, &MainWindow::onSingleInstanceConnect);
    if (instanceServer->listen(serverName)) {
        /* Remove server in case of crashes */
        if (instanceServer->serverError() == QAbstractSocket::AddressInUseError &&
                QFile::exists(instanceServer->serverName())) {
            QFile::remove(instanceServer->serverName());
            instanceServer->listen(serverName);
        }
    }
}

void MainWindow::onSingleInstanceConnect()
{
    QLocalSocket *socket = instanceServer->nextPendingConnection();
    if (!socket) {
        return;
    }

    if (socket->waitForReadyRead(1000)) {
        QByteArray username = qgetenv("USER");
        if (username.isEmpty()) {
            username = qgetenv("USERNAME");
        }

        QByteArray recvUsername = socket->readAll();
        if (recvUsername == username) {
            // Only show the window if it's the same user
            show();
        } else {
            qWarning("Another user is trying to run another instance of ss-qt5");
        }
    }
    socket->deleteLater();
}
