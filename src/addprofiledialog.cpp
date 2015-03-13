#include <QScreen>
#include <QDesktopWidget>
#include <QtConcurrent>
#include <QFileDialog>
#include <zbar.h>
#include "addprofiledialog.h"
#include "ssvalidator.h"
#include "ui_addprofiledialog.h"

AddProfileDialog::AddProfileDialog(bool _enforce, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddProfileDialog),
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

    connect(ui->profileNameEdit, &QLineEdit::textChanged, this, &AddProfileDialog::onProfileNameChanged);
    connect(ui->scanButton, &QPushButton::clicked, this, &AddProfileDialog::onScanButtonClicked);
    connect(ui->qrfileButton, &QPushButton::clicked, this, &AddProfileDialog::onQRFileButtonClicked);
    connect(ui->ssuriEdit, &QLineEdit::textChanged, this, &AddProfileDialog::checkBase64SSURI);
    connect(ui->cancelButton, &QPushButton::clicked, this, &AddProfileDialog::onRejected);
    connect(ui->addButton, &QPushButton::clicked, this, &AddProfileDialog::onAccepted);
    connect(ui->ssuriCheckBox, &QCheckBox::toggled, this, &AddProfileDialog::checkIsValid);
}

AddProfileDialog::~AddProfileDialog()
{
    delete ui;
}

QImage AddProfileDialog::convertToGrey(const QImage &input)
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

void AddProfileDialog::setupURIfromQRImg(const QImage &qrimg)
{
    QImage gimg = convertToGrey(qrimg);

    //use zbar to decode the QR code
    zbar::ImageScanner scanner;
    zbar::Image image(gimg.width(), gimg.height(), "Y800", gimg.bits(), gimg.byteCount());
    scanner.scan(image);
    zbar::SymbolSet res_set = scanner.get_results();
    for (zbar::SymbolIterator it = res_set.symbol_begin(); it != res_set.symbol_end(); ++it) {
        if (it->get_type() == zbar::ZBAR_QRCODE) {
            /*
             * uri will be overwritten if the result is valid
             * this means always the last uri gets used
             * therefore, please only leave one QR code for the sake of accuracy
             */
            QString result = QString::fromStdString(it->get_data());
            if (result.left(5).compare("ss://", Qt::CaseInsensitive) == 0) {
                ui->ssuriEdit->setText(result);
            }
        }
    }
}

void AddProfileDialog::onProfileNameChanged(const QString &name)
{
    validName = !name.isEmpty();
    checkIsValid();
}

void AddProfileDialog::onScanButtonClicked()
{
    /*
     * Somehow using QtConcurrent::map will lead to crash.
     * Therefore, just run parsing part in another thread instead of concurrently
     */
    QFuture<void> future = QtConcurrent::run([&]{
        QList<QScreen *> screens = qApp->screens();
        for (QList<QScreen *>::iterator sc = screens.begin(); sc != screens.end(); ++sc) {
            QImage raw_sc = (*sc)->grabWindow(qApp->desktop()->winId()).toImage();
            setupURIfromQRImg(raw_sc);
        }
    });
    fw->setFuture(future);
}

void AddProfileDialog::onQRFileButtonClicked()
{
    QString imgFile = QFileDialog::getOpenFileName(this, tr("Open QR Code Image File"), QString(), "Images (*.png *.xpm *.jpg *.jpeg)");
    if (!imgFile.isEmpty()) {
        QImage img(imgFile);
        setupURIfromQRImg(img);
    }
}

void AddProfileDialog::checkBase64SSURI(const QString &str)
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

void AddProfileDialog::checkIsValid()
{
    if (ui->ssuriCheckBox->isChecked()) {
        ui->addButton->setEnabled(validName && validURI);
    }
    else {
        ui->addButton->setEnabled(validName);
    }
}

void AddProfileDialog::onAccepted()
{
    emit inputAccepted(ui->profileNameEdit->text(), ui->ssuriCheckBox->isChecked(), ui->ssuriEdit->text());
    this->accept();
}

void AddProfileDialog::onRejected()
{
    emit inputRejected(enforce);
    this->reject();
}
