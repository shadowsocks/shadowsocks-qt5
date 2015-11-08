#include "ssvalidator.h"

const QStringList SSValidator::supportedMethod = QStringList() << "RC4-MD5" << "AES-128-CFB" << "AES-192-CFB" << "AES-256-CFB" << "BF-CFB" << "CAMELLIA-128-CFB" << "CAMELLIA-192-CFB" << "CAMELLIA-256-CFB" << "CAST5-CFB" << "CHACHA20"<< "DES-CFB" << "IDEA-CFB" << "RC2-CFB" << "SALSA20" << "SEED-CFB";//all upper-case

bool SSValidator::validate(QString input)
{
    //must begin with ss:// to distinguish from random base64 encoded strings
    if (input.startsWith("ss://")) {
        input.remove(0, 5);
        QString decode(QByteArray::fromBase64(QByteArray(input.toStdString().c_str())));
        QStringList decList = decode.split(':');
        if (decList.size() < 3) {
            return false;
        }

        //Validate Method
        QString method = decList.first();
        if (!validateMethod(method)) {
            return false;
        }

        //Validate Port
        QString port = decList.last();
        if (!validatePort(port)) {
            return false;
        }

        //Validate whether server and password exist
        QStringList pwdServer = decList.at(1).split('@');
        if (pwdServer.size() < 2) {
            return false;
        }

        //it seems acceptable now
        return true;
    }
    else {
        return false;
    }
}

bool SSValidator::validatePort(const QString &port)
{
    bool ok;
    port.toUShort(&ok);
    return ok;
}

bool SSValidator::validateMethod(const QString &method)
{
    return supportedMethod.contains(method, Qt::CaseInsensitive);
}
