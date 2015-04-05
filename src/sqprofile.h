#ifndef SQPROFILE_H
#define SQPROFILE_H

#include <QDataStream>
#include <QDateTime>

struct SQProfile
{
    SQProfile();

    bool autoStart;
    bool debug;
    quint16 serverPort;
    quint16 localPort;
    QString name;
    QString serverAddress;
    QString localAddress;
    QString method;
    QString password;
    int timeout;
    int lag;
    QDateTime lastTime;
};
Q_DECLARE_METATYPE(SQProfile)

QDataStream& operator << (QDataStream &out, const SQProfile &p);
QDataStream& operator >> (QDataStream &in, SQProfile &p);

#endif // SQPROFILE_H
