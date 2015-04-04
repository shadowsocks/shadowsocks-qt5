#include "editdialog.h"
#include "ui_editdialog.h"
#include "ssvalidator.h"
#include "ip4validator.h"
#include "portvalidator.h"

EditDialog::EditDialog(Connection *_connection, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDialog),
    connection(_connection)
{
    ui->setupUi(this);

    /* initialisation and validator setup */
    ui->encryptComboBox->addItems(SSValidator::supportedMethod);
    IP4Validator *addrValidator = new IP4Validator(this);
    PortValidator *portValidator = new PortValidator(this);
    ui->serverPortEdit->setValidator(portValidator);
    ui->localPortEdit->setValidator(portValidator);
    //Maybe we shouldn't validate local address using IPv4 format?
    ui->localAddrEdit->setValidator(addrValidator);

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
    connection->profile.server_port = ui->serverPortEdit->text().toUShort();
    connection->profile.local_address = ui->localAddrEdit->text();
    connection->profile.local_port = ui->localPortEdit->text().toUShort();
    connection->profile.password = ui->pwdEdit->text();
    connection->profile.method = ui->encryptComboBox->currentText();
    connection->profile.timeout = ui->timeoutSpinBox->value();
    connection->debug = ui->debugCheckBox->isChecked();
}
