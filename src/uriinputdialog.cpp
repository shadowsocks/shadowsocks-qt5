#include "uriinputdialog.h"
#include "ui_uriinputdialog.h"
#include "ssvalidator.h"
#include <QPushButton>

URIInputDialog::URIInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::URIInputDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    connect(ui->uriEdit, &QLineEdit::textChanged, this, &URIInputDialog::onURIChanged);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &URIInputDialog::onAccepted);

    this->adjustSize();
}

URIInputDialog::~URIInputDialog()
{
    delete ui;
}

void URIInputDialog::onURIChanged(const QString &str)
{
    if (!SSValidator::validate(str)) {
        ui->uriEdit->setStyleSheet("background: pink");
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
    else {
        ui->uriEdit->setStyleSheet("background: #81F279");
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

void URIInputDialog::onAccepted()
{
    emit acceptedURI(ui->uriEdit->text());
    this->accept();
}
