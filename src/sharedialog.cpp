#include <QFileDialog>
#include "qrwidget.h"
#include "sharedialog.h"
#include "ui_sharedialog.h"

ShareDialog::ShareDialog(const QByteArray &ssUrl, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShareDialog)
{
    ui->setupUi(this);
    ui->qrWidget->setQRData(ssUrl);
    ui->ssUrlEdit->setText(QString(ssUrl));
    ui->ssUrlEdit->setCursorPosition(0);

    connect(ui->saveButton, &QPushButton::clicked, this, &ShareDialog::onSaveButtonClicked);

    this->adjustSize();
}

ShareDialog::~ShareDialog()
{
    delete ui;
}

void ShareDialog::onSaveButtonClicked()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save QR Code"), QString(), "PNG (*.png)");
    if (!filename.isEmpty()) {
        ui->qrWidget->getQRImage().save(filename);
    }
}
