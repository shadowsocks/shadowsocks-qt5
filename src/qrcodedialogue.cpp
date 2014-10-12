#include "qrwidget.h"
#include "qrcodedialogue.h"
#include "ui_qrcodedialogue.h"

QRCodeDialogue::QRCodeDialogue(const QByteArray &ssUrl, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QRCodeDialogue)
{
    ui->setupUi(this);
    ui->qrWidget->setQRData(ssUrl);
    ui->ssUrlEdit->setText(QString(ssUrl));
}

QRCodeDialogue::~QRCodeDialogue()
{
    delete ui;
}
