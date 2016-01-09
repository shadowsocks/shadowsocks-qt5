#include "qrcodecapturer.h"
#include "urihelper.h"
#include <QApplication>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QDesktopWidget>
#include <QScreen>

QRCodeCapturer::QRCodeCapturer(QWidget *parent) :
    QMainWindow(parent)
{
#ifdef Q_OS_WIN
    /*
     * On Windows, it requires Qt::FramelessWindowHint to be set to make
     * translucent background work, but we need a window with borders.
     * Therefore, we set the entire window semi-transparent so that
     * users are still able to see the region below while moving the
     * capturer above the QR code image.
     */
    this->setWindowOpacity(0.75);
#else
    this->setAttribute(Qt::WA_TranslucentBackground, true);
#endif
    this->setWindowTitle(tr("QR Capturer"));
    this->setMinimumSize(400, 400);
}

QRCodeCapturer::~QRCodeCapturer()
{}

QString QRCodeCapturer::scanEntireScreen()
{
    QString uri;
    QList<QScreen *> screens = qApp->screens();
    for (QList<QScreen *>::iterator sc = screens.begin();
         sc != screens.end();
         ++sc) {
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
    QMainWindow::moveEvent(e);
    decodeCurrentRegion();
}

void QRCodeCapturer::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    decodeCurrentRegion();
}

void QRCodeCapturer::closeEvent(QCloseEvent *e)
{
    QMainWindow::closeEvent(e);
    emit closed();
}

void QRCodeCapturer::decodeCurrentRegion()
{
    QScreen *sc = qApp->screens().at(qApp->desktop()->screenNumber(this));
    QRect geometry = this->geometry();
    QImage raw_sc = sc->grabWindow(qApp->desktop()->winId(),
                                   geometry.x(),
                                   geometry.y(),
                                   geometry.width(),
                                   geometry.height()).toImage();
    QString result = URIHelper::decodeImage(raw_sc);
    if (!result.isNull()) {
        this->close();
        // moveEvent and resizeEvent both happen quite frequent
        // it's very likely this signal would be emitted multiple times
        // the solution is to use Qt::DirectConnection signal-slot connection
        // and disconnect such a connection in the slot function
        emit qrCodeFound(result);
    }
}
