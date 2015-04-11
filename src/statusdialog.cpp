#include "statusdialog.h"
#include "ui_statusdialog.h"

StatusDialog::StatusDialog(Connection *c, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatusDialog),
    con(c)
{
    ui->setupUi(this);

    ui->nameLabel->setText(con->getName());
    ui->serverAddrLabel->setText(con->profile.serverAddress);
    ui->serverPortLabel->setText(QString::number(con->profile.serverPort));
    ui->localPortLabel->setText(QString::number(con->profile.localPort));
    ui->statusLabel->setText(con->isRunning() ? tr("Connected") : tr("Disconnected"));
    onBytesReadChanged(con->profile.bytesRead);
    onBytesSentChanged(con->profile.bytesSent);

    connect(con, &Connection::stateChanged, this, &StatusDialog::onStatusChanged);
    connect(con, &Connection::bytesReadChanged, this, &StatusDialog::onBytesReadChanged);
    connect(con, &Connection::bytesSentChanged, this, &StatusDialog::onBytesSentChanged);
    connect(ui->resetButton, &QPushButton::clicked, this, &StatusDialog::onResetClicked);

    this->adjustSize();
}

StatusDialog::~StatusDialog()
{
    delete ui;
}

const QStringList StatusDialog::units = QStringList() << " B" << " KiB" << " MiB" << " GiB" << " TiB" << " PiB" << " EiB" << " ZiB" << " YiB";

void StatusDialog::onStatusChanged(bool b)
{
    ui->statusLabel->setText(b ? tr("Connected") : tr("Disconnected"));
}

void StatusDialog::onBytesReadChanged(const quint64 &b)
{
    ui->brLabel->setText(convertToHumanReadable(b));
}

void StatusDialog::onBytesSentChanged(const quint64 &b)
{
    ui->bsLabel->setText(convertToHumanReadable(b));
}

QString StatusDialog::convertToHumanReadable(quint64 bytes)
{
    int unitId = 0;
    for (; bytes > 1024; bytes /= 1024, unitId++);
    QString str = QString::number(bytes) + units.at(unitId);
    return str;
}

void StatusDialog::onResetClicked()
{
    con->profile.bytesRead = 0;
    con->profile.bytesSent = 0;
    onBytesReadChanged(0);
    onBytesSentChanged(0);
}
