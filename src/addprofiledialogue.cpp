#include "addprofiledialogue.h"
#include "ssvalidator.h"
#include "ui_addprofiledialogue.h"

AddProfileDialogue::AddProfileDialogue(QWidget *parent, bool e) :
    QDialog(parent),
    ui(new Ui::AddProfileDialogue)
{
    ui->setupUi(this);
    enforce = e;
    connect(ui->ssuriEdit, &QLineEdit::textChanged, this, &AddProfileDialogue::checkBase64SSURI);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AddProfileDialogue::onAccepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &AddProfileDialogue::onRejected);
}

AddProfileDialogue::~AddProfileDialogue()
{
    delete ui;
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
