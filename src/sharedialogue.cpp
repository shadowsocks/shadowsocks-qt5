#include <QFileDialog>
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
    ui->ssUrlEdit->setCursorPosition(0);

    connect(ui->saveButton, &QPushButton::clicked, this, &ShareDialogue::onSaveButtonClicked);
}

ShareDialogue::~ShareDialogue()
{
    delete ui;
}

void ShareDialogue::onSaveButtonClicked()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save QR Code"), QString(), "PNG (*.png)");
    if (!filename.isEmpty()) {
        ui->qrWidget->getQRImage().save(filename);
    }
}
