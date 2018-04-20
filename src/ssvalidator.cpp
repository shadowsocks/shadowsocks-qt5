#include "ssvalidator.h"
#include <QtShadowsocks>

QStringList SSValidator::supportedMethodList()
{
    std::vector<std::string> methodBA = QSS::Cipher::supportedMethods();
    std::sort(methodBA.begin(), methodBA.end());
    QStringList methodList;
    for (const std::string& method : methodBA) {
        methodList.push_back(QString::fromStdString(method).toUpper());
    }
    return methodList;
}

bool SSValidator::validate(const QString &input)
{
    bool valid = true;
    try {
        QSS::Profile::fromUri(input.toStdString());
    } catch(const std::exception&) {
        valid = false;
    }
    return valid;
}

bool SSValidator::validatePort(const QString &port)
{
    bool ok;
    port.toUShort(&ok);
    return ok;
}

bool SSValidator::validateMethod(const QString &method)
{
    static const QStringList validMethodList = supportedMethodList();
    return validMethodList.contains(method, Qt::CaseInsensitive);
}
