#include "ip4validator.h"

IP4Validator::IP4Validator(QObject *parent)
    : QValidator(parent)
{}

QValidator::State IP4Validator::validate(QString &input, int &pos) const
{
    Q_UNUSED(pos);
    if (input.isEmpty()) {
        return Acceptable;
    }

    QStringList slist = input.split(".");
    int number = slist.size();
    if (number > 4) {
        return Invalid;
    }

    bool emptyGroup = false;
    for (int i = 0; i < number; i++) {
        bool ok;
        if(slist[i].isEmpty()){
          emptyGroup = true;
          continue;
        }
        int value = slist[i].toInt(&ok);
        if(!ok || value < 0 || value > 255) {
            return Invalid;
        }
    }

    if(number < 4 || emptyGroup) {
        return Intermediate;
    }

    return Acceptable;
}
