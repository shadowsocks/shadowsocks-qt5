#include "editdialog.h"
#include "ui_editdialog.h"

EditDialog::EditDialog(Connection *_connection, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDialog),
    connection(_connection)
{
    ui->setupUi(this);

    ui->nameEdit->setText(connection->name);
    ui->serverAddrEdit->setText(connection->profile.server);
    ui->serverPortEdit->setText(QString::number(connection->profile.server_port));
    ui->localAddrEdit->setText(connection->profile.local_address);
    ui->localPortEdit->setText(QString::number(connection->profile.local_port));
    ui->pwdEdit->setText(connection->profile.password);
    ui->encryptComboBox->setCurrentText(connection->profile.method.toUpper());
    ui->timeoutSpinBox->setValue(connection->profile.timeout);
    ui->debugCheckBox->setChecked(connection->debug);
}

EditDialog::~EditDialog()
{
    delete ui;
}

void EditDialog::save()
{
    connection->name = ui->nameEdit->text();
    connection->profile.server = ui->serverAddrEdit->text();
    connection->profile.server_port = ui->serverPortEdit->text().toInt();
    connection->profile.local_address = ui->localAddrEdit->text();
    connection->profile.local_port = ui->localPortEdit->text().toInt();
    connection->profile.password = ui->pwdEdit->text();
    connection->profile.method = ui->encryptComboBox->currentText();
    connection->profile.timeout = ui->timeoutSpinBox->value();
    connection->debug = ui->debugCheckBox->isChecked();
}
