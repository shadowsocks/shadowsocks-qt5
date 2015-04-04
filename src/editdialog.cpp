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
    ui->serverAddrEdit->setText(connection->serverAddress);
    ui->serverPortEdit->setText(QString::number(connection->serverPort));
    ui->localAddrEdit->setText(connection->localAddress);
    ui->localPortEdit->setText(QString::number(connection->localPort));
    ui->pwdEdit->setText(connection->password);
    ui->encryptComboBox->setCurrentText(connection->method.toUpper());
    ui->timeoutSpinBox->setValue(connection->timeout);
    ui->debugCheckBox->setChecked(connection->debug);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &EditDialog::save);
}

EditDialog::~EditDialog()
{
    delete ui;
}

void EditDialog::save()
{
    connection->name = ui->nameEdit->text();
    connection->serverAddress = ui->serverAddrEdit->text();
    connection->serverPort = ui->serverPortEdit->text().toUShort();
    connection->localAddress = ui->localAddrEdit->text();
    connection->localPort = ui->localPortEdit->text().toUShort();
    connection->password = ui->pwdEdit->text();
    connection->method = ui->encryptComboBox->currentText();
    connection->timeout = ui->timeoutSpinBox->value();
    connection->debug = ui->debugCheckBox->isChecked();
    this->accept();
}
