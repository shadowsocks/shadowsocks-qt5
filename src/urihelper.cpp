#include "urihelper.h"
#include <zbar.h>

QImage URIHelper::convertToGrey(const QImage &input)
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

QString URIHelper::decodeImage(const QImage &img)
{
    QString uri;
    QImage gimg = convertToGrey(img);

    //use zbar to decode the QR code
    zbar::ImageScanner scanner;
    zbar::Image image(gimg.bytesPerLine(), gimg.height(), "Y800", gimg.bits(), gimg.byteCount());
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
                uri = result;
            }
        }
    }

    return uri;
}
