/*
 * This validator is ONLY used to validate a string that is already finished editing.
 * Hence, it won't return Intermediate but only Invalid or Accpetance.
 */

#ifndef SSVALIDATOR_H
#define SSVALIDATOR_H
#include <QString>
#include <QStringList>

class SSValidator
{
public:
    SSValidator();
    static bool validate(QString input);
    static bool validatePort(const QString &port);
    static bool validateMethod(const QString &method);
    static const QStringList supportedMethod;
};

#endif // SSVALIDATOR_H
