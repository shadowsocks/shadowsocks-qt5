#ifndef PORTVALIDATOR_H
#define PORTVALIDATOR_H

#include <QValidator>

class PortValidator : public QValidator
{
public:
    PortValidator(QObject *parent = 0);
    State validate(QString &input, int &) const;
};

#endif // PORTVALIDATOR_H
