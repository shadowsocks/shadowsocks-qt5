#include <QPainter>
#include <QStyleOption>
#include <QDebug>
#include <qrencode.h>
#include "qrwidget.h"

QRWidget::QRWidget(QWidget *parent) :
    QWidget(parent)
{}

void QRWidget::setQRData(const QByteArray &data)
{
    qrImage = QImage(512, 512, QImage::Format_Mono);
    QPainter painter(&qrImage);
    QRcode *qrcode = QRcode_encodeString(data.constData(), 1, QR_ECLEVEL_L, QR_MODE_8, 1);
    if (qrcode) {
        QColor fg(Qt::black);
        QColor bg(Qt::white);
        painter.setBrush(bg);
        painter.setPen(Qt::NoPen);
        painter.drawRect(0, 0, 512, 512);
        painter.setBrush(fg);
        const int s = qrcode->width > 0 ? qrcode->width : 1;
        const qreal scale = 512.0 / s;
        for(int y = 0; y < s; y++){
            for(int x = 0; x < s; x++){
                if(qrcode->data[y * s + x] & 0x01){
                    const qreal rx1 = x * scale, ry1 = y * scale;
                    QRectF r(rx1, ry1, scale, scale);
                    painter.drawRects(&r,1);
                }
            }
        }
        QRcode_free(qrcode);
    }
    else {
        qWarning() << tr("Generating QR code failed.");
    }
}

void QRWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    QSizeF nSize = qrImage.size().scaled(this->size(), Qt::KeepAspectRatio);
    painter.translate((this->width() - nSize.width()) / 2, (this->height() - nSize.height()) / 2);
    painter.scale(nSize.width() / qrImage.width(), nSize.height() / qrImage.height());
    painter.drawImage(0, 0, qrImage);
}

const QImage& QRWidget::getQRImage() const
{
    return qrImage;
}
