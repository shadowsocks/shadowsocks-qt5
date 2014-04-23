#include "addprofiledialogue.h"
#include "ssvalidator.h"
#include "ui_addprofiledialogue.h"

AddProfileDialogue::AddProfileDialogue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddProfileDialogue)
{
    ui->setupUi(this);
    enforce = false;
    connect(ui->ssuriEdit, &QLineEdit::textChanged, this, &AddProfileDialogue::checkBase64SSURI);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AddProfileDialogue::onAccepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &AddProfileDialogue::onRejected);
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);//no min/max/close buttons
}

AddProfileDialogue::~AddProfileDialogue()
{
    delete ui;
}

void AddProfileDialogue::setEnforceMode(bool e)
{
    enforce = e;
}

void AddProfileDialogue::clear()
{
    ui->profileNameEdit->clear();
    ui->ssuriEdit->clear();
    ui->ssuriEdit->setStyleSheet("");
    ui->ssuriCheckBox->setChecked(false);
}

void AddProfileDialogue::checkBase64SSURI(const QString &str)
{
    if (!SSValidator::validate(str)) {
        ui->ssuriEdit->setStyleSheet("background: pink");
        ui->buttonBox->setEnabled(false);
    }
    else {
        ui->ssuriEdit->setStyleSheet("background: #81F279");
        ui->buttonBox->setEnabled(true);
    }
}

void AddProfileDialogue::onAccepted()
{
    emit inputAccepted(ui->profileNameEdit->text(), ui->ssuriCheckBox->isChecked(), ui->ssuriEdit->text());
}

void AddProfileDialogue::onRejected()
{
    emit inputRejected(enforce);
}
