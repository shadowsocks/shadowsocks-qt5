#include "addprofiledialogue.h"
#include "ui_addprofiledialogue.h"

AddProfileDialogue::AddProfileDialogue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddProfileDialogue)
{
    ui->setupUi(this);
    enforce = false;
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AddProfileDialogue::onAccepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &AddProfileDialogue::onRejected);
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
    ui->ssuriCheckBox->setChecked(false);
}

void AddProfileDialogue::onAccepted()
{
    emit inputAccepted(ui->profileNameEdit->text(), ui->ssuriCheckBox->isChecked(), ui->ssuriEdit->text());
}

void AddProfileDialogue::onRejected()
{
    emit inputRejected(enforce);
}
