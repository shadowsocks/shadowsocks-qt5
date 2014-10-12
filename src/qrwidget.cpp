#include <QPainter>
#include <QDebug>
#include <qrencode.h>
#include "qrwidget.h"

QRWidget::QRWidget(QWidget *parent) :
    QWidget(parent)
{
    data = "http://www.shadowsocks.org";
}

void QRWidget::setQRData(const QByteArray &qrData)
{
    data = qrData;
}

void QRWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QRcode *qrcode = QRcode_encodeString(data.constData(), 1, QR_ECLEVEL_L, QR_MODE_8, 1);
    if (qrcode != NULL) {
        QColor fg(Qt::black);
        QColor bg(Qt::white);
        painter.setBrush(bg);
        painter.setPen(Qt::NoPen);
        const double w = width();
        const double h = height();
        painter.drawRect(0, 0, w, h);
        painter.setBrush(fg);
        const int s = qrcode->width > 0 ? qrcode->width : 1;
        const double aspect = w / h;
        const double scale = ((aspect > 1.0) ? h : w) / s;
        for(int y = 0; y < s; y++){
            const int yy = y * s;
            for(int x = 0; x < s; x++){
                const int xx = yy + x;
                const unsigned char b = qrcode->data[xx];
                if(b &0x01){
                    const double rx1 = x * scale, ry1 = y * scale;
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
