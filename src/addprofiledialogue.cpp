#include <QScreen>
#include <QDesktopWidget>
#include <zbar.h>
#include "addprofiledialogue.h"
#include "ssvalidator.h"
#include "ui_addprofiledialogue.h"

AddProfileDialogue::AddProfileDialogue(QWidget *parent, bool e) :
    QDialog(parent),
    ui(new Ui::AddProfileDialogue)
{
    ui->setupUi(this);
    enforce = e;
    connect(ui->scanButton, &QPushButton::clicked, this, &AddProfileDialogue::onScanButtonClicked);
    connect(ui->ssuriEdit, &QLineEdit::textChanged, this, &AddProfileDialogue::checkBase64SSURI);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AddProfileDialogue::onAccepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &AddProfileDialogue::onRejected);
}

AddProfileDialogue::~AddProfileDialogue()
{
    delete ui;
}

void AddProfileDialogue::onScanButtonClicked()
{
    /*
     * capture the whole screen
     */
    QImage screenshot = qApp->screens().at(0)->grabWindow(qApp->desktop()->winId()).toImage().convertToFormat(QImage::Format_Indexed8);

    //use zbar to decode the QR code, if found.
    QString uri;
    zbar::ImageScanner scanner;
    zbar::Image image(screenshot.width(), screenshot.height(), "Y800", screenshot.bits(), screenshot.byteCount());
    scanner.scan(image);
    zbar::SymbolSet res_set = scanner.get_results();
    for (zbar::SymbolIterator it = res_set.symbol_begin(); it != res_set.symbol_end(); ++it) {
        if (it->get_type() == zbar::ZBAR_QRCODE) {
            uri = QString::fromStdString(it->get_data());
        }
    }

    //update the SS URI if QR code is decoded successfully
    if (!uri.isEmpty()) {
        ui->ssuriEdit->setText(uri);
    }
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
