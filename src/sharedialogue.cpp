#include "qrwidget.h"
#include "sharedialogue.h"
#include "ui_sharedialogue.h"

ShareDialogue::ShareDialogue(const QByteArray &ssUrl, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShareDialogue)
{
    ui->setupUi(this);
    ui->qrWidget->setQRData(ssUrl);
    ui->ssUrlEdit->setText(QString(ssUrl));
}

ShareDialogue::~ShareDialogue()
{
    delete ui;
}
