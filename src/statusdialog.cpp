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
    ui->statusLabel->setText(con->isRunning() ? tr("Connected") : tr("Disconnected"));
    ui->brLabel->setText(QString::number(con->profile.bytesRead));
    ui->bsLabel->setText(QString::number(con->profile.bytesSent));

    connect(con, &Connection::stateChanged, this, &StatusDialog::onStatusChanged);
}

StatusDialog::~StatusDialog()
{
    delete ui;
}

void StatusDialog::onStatusChanged(bool b)
{
    ui->statusLabel->setText(b ? tr("Connected") : tr("Disconnected"));
}

void StatusDialog::onBytesReadChanged(const quint64 &b)
{
    ui->brLabel->setText(QString::number(b));
}

void StatusDialog::onBytesSentChanged(const quint64 &b)
{
    ui->bsLabel->setText(QString::number(b));
}
