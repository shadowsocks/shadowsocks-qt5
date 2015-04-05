#ifndef URIHELPER_H
#define URIHELPER_H

#include <QString>
#include <QImage>

class URIHelper
{
public:
    virtual ~URIHelper() = 0;

    static QImage convertToGrey(const QImage &input);
    static QString decodeImage(const QImage &img);
};

#endif // URIHELPER_H
