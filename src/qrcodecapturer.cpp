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
#ifdef Q_OS_WIN
    /*
     * On Windows, it requires Qt::FramelessWindowHint to be set to make translucent background work, but we need a window with borders.
     * Therefore, we set the entire window semi-transparent so that users are still able to see the region below while moving the
     * capturer above the QR code image.
     */
    this->setWindowOpacity(0.75);
#else
    this->setAttribute(Qt::WA_TranslucentBackground, true);
#endif
}

QRCodeCapturer::~QRCodeCapturer()
{
    delete ui;
}

QString QRCodeCapturer::scanEntireScreen()
{
    QString uri;
    QList<QScreen *> screens = qApp->screens();
    for (QList<QScreen *>::iterator sc = screens.begin(); sc != screens.end(); ++sc) {
        QImage raw_sc = (*sc)->grabWindow(qApp->desktop()->winId()).toImage();
        QString result = URIHelper::decodeImage(raw_sc);
        if (!result.isNull()) {
            uri = result;
        }
    }
    return uri;
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
