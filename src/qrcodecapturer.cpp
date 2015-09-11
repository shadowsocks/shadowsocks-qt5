#include "qrcodecapturer.h"
#include "ui_qrcodecapturer.h"
#include "urihelper.h"
#include <QMoveEvent>
#include <QResizeEvent>
#include <QDesktopWidget>
#include <QScreen>

QRCodeCapturer::QRCodeCapturer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QRCodeCapturer)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
}

QRCodeCapturer::~QRCodeCapturer()
{
    delete ui;
}

void QRCodeCapturer::moveEvent(QMoveEvent *e)
{
    QDialog::moveEvent(e);
    decodeCurrentRegion();
}

void QRCodeCapturer::resizeEvent(QResizeEvent *e)
{
    QDialog::resizeEvent(e);
    decodeCurrentRegion();
}

void QRCodeCapturer::decodeCurrentRegion()
{
    QScreen *sc = qApp->screens().at(qApp->desktop()->screenNumber(this));
    QRect geometry = this->geometry();
    QImage raw_sc = sc->grabWindow(qApp->desktop()->winId(), geometry.x(), geometry.y(), geometry.width(), geometry.height()).toImage();
    QString result = URIHelper::decodeImage(raw_sc);
    if (!result.isNull()) {
        this->hide();
        emit qrCodeFound(result);
        this->accept();
    }
}
