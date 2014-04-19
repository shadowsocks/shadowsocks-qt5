#include "portvalidator.h"

PortValidator::PortValidator(QObject *parent)
    : QValidator(parent)
{}

QValidator::State PortValidator::validate(QString &input, int &) const
{
    bool ok;
    int v = input.toInt(&ok);
    if (!ok || v < 1 || v > 65535) {
        return Invalid;
    }
    else
        return Acceptable;
}
