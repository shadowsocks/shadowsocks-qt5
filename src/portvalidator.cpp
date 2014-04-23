#include "portvalidator.h"
#include "ssvalidator.h"

PortValidator::PortValidator(QObject *parent)
    : QValidator(parent)
{}

QValidator::State PortValidator::validate(QString &input, int &) const
{
    if (SSValidator::validatePort(input)) {
        return Acceptable;
    }
    else
        return Invalid;
}
