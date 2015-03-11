#include <QScreen>
#include <QDesktopWidget>
#include <QtConcurrent>
#include <zbar.h>
#include "addprofiledialogue.h"
#include "ssvalidator.h"
#include "ui_addprofiledialogue.h"

AddProfileDialogue::AddProfileDialogue(bool _enforce, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddProfileDialogue),
    enforce(_enforce)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);//progress bar is used as a QR code scanning busy indicator

    validName = false;
    validURI = false;

    fw = new QFutureWatcher<void>(this);
    connect(fw, &QFutureWatcher<void>::started, [&]{
        ui->progressBar->setVisible(true);
        ui->scanButton->setEnabled(false);
        ui->ssuriCheckBox->setEnabled(false);
    });
    connect(fw, &QFutureWatcher<void>::finished, [&]{
        ui->progressBar->setVisible(false);
        ui->scanButton->setEnabled(true);
        ui->ssuriCheckBox->setEnabled(true);
    });

    connect(ui->profileNameEdit, &QLineEdit::textChanged, this, &AddProfileDialogue::onProfileNameChanged);
    connect(ui->scanButton, &QPushButton::clicked, this, &AddProfileDialogue::onScanButtonClicked);
    connect(ui->ssuriEdit, &QLineEdit::textChanged, this, &AddProfileDialogue::checkBase64SSURI);
    connect(ui->cancelButton, &QPushButton::clicked, this, &AddProfileDialogue::onRejected);
    connect(ui->addButton, &QPushButton::clicked, this, &AddProfileDialogue::onAccepted);
    connect(ui->ssuriCheckBox, &QCheckBox::toggled, this, &AddProfileDialogue::checkIsValid);
}

AddProfileDialogue::~AddProfileDialogue()
{
    delete ui;
}

QImage AddProfileDialogue::convertToGrey(const QImage &input)
{
    if (input.isNull()) {
        return QImage();
    }
    QImage ret(input.width(), input.height(), QImage::Format_Indexed8);
    QVector<QRgb> gtable(256);
    for (int i = 0; i < 256; ++i) {
        gtable[i] = qRgb(i, i, i);
    }
    ret.setColorTable(gtable);
    for (int i = 0; i < input.width(); ++i) {
        for (int j = 0; j < input.height(); ++j) {
            QRgb val = input.pixel(i, j);
            ret.setPixel(i, j, qGray(val));
        }
    }
    return ret;
}

void AddProfileDialogue::onProfileNameChanged(const QString &name)
{
    validName = !name.isEmpty();
    checkIsValid();
}

void AddProfileDialogue::onScanButtonClicked()
{
    /*
     * Somehow using QtConcurrent::map will lead to crash.
     * Therefore, just run parsing part in another thread instead of concurrently
     */
    QFuture<void> future = QtConcurrent::run([&]{
        QList<QScreen *> screens = qApp->screens();
        for (QList<QScreen *>::iterator sc = screens.begin(); sc != screens.end(); ++sc) {
            QImage raw_sc = (*sc)->grabWindow(qApp->desktop()->winId()).toImage();
            QImage screenshot = convertToGrey(raw_sc);

            //use zbar to decode the QR code, if found.
            zbar::ImageScanner scanner;
            zbar::Image image(screenshot.width(), screenshot.height(), "Y800", screenshot.bits(), screenshot.byteCount());
            scanner.scan(image);
            zbar::SymbolSet res_set = scanner.get_results();
            for (zbar::SymbolIterator it = res_set.symbol_begin(); it != res_set.symbol_end(); ++it) {
                if (it->get_type() == zbar::ZBAR_QRCODE) {
                    /*
                     * uri will be overwritten if the result is valid
                     * this means always the last uri gets used
                     * therefore, please only leave one QR code on all screens for accuracy
                     */
                    QString result = QString::fromStdString(it->get_data());
                    if (result.left(5).compare("ss://", Qt::CaseInsensitive) == 0) {
                        ui->ssuriEdit->setText(result);
                    }
                }
            }
        }
    });
    fw->setFuture(future);
}

void AddProfileDialogue::checkBase64SSURI(const QString &str)
{
    if (!SSValidator::validate(str)) {
        ui->ssuriEdit->setStyleSheet("background: pink");
        validURI = false;
    }
    else {
        ui->ssuriEdit->setStyleSheet("background: #81F279");
        validURI = true;
    }
    checkIsValid();
}

void AddProfileDialogue::checkIsValid()
{
    if (ui->ssuriCheckBox->isChecked()) {
        ui->addButton->setEnabled(validName && validURI);
    }
    else {
        ui->addButton->setEnabled(validName);
    }
}

void AddProfileDialogue::onAccepted()
{
    emit inputAccepted(ui->profileNameEdit->text(), ui->ssuriCheckBox->isChecked(), ui->ssuriEdit->text());
    this->accept();
}

void AddProfileDialogue::onRejected()
{
    emit inputRejected(enforce);
    this->reject();
}
