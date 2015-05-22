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
    onBytesChanged(con->profile.currentUsage, con->profile.totalUsage);

    connect(con, &Connection::stateChanged, this, &StatusDialog::onStatusChanged);
    connect(con, &Connection::dataUsageChanged, this, &StatusDialog::onBytesChanged);
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

void StatusDialog::onBytesChanged(const quint64 &current, const quint64 &total)
{
    ui->usageTermLabel->setText(convertToHumanReadable(current));
    ui->usageTotalLabel->setText(convertToHumanReadable(total));
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
    con->profile.currentUsage = 0;
    con->profile.totalUsage = 0;
    onBytesChanged(0, 0);
}
