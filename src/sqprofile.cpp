#include "sqprofile.h"

SQProfile::SQProfile()
{
    autoStart = false;
    debug = false;
    serverPort = 8388;
    localPort = 1080;
    name = QObject::tr("Unamed Profile");
    localAddress = QString("127.0.0.1");
    method = QString("RC4-MD5");
    timeout = 600;
    lag = -3;//-1: timeout, -2: error, -3: unknown(untested)
    bytesRead = 0;
    bytesSent = 0;
}

QDataStream& operator << (QDataStream &out, const SQProfile &p)
{
    out << p.autoStart << p.debug << p.serverPort << p.localPort << p.name << p.serverAddress << p.localAddress << p.method << p.password << p.timeout << p.lag << p.bytesRead << p.bytesSent << p.lastTime;
    return out;
}

QDataStream& operator >> (QDataStream &in, SQProfile &p)
{
    in >> p.autoStart >> p.debug >> p.serverPort >> p.localPort >> p.name >> p.serverAddress >> p.localAddress >> p.method >> p.password >> p.timeout >> p.lag >> p.bytesRead >> p.bytesSent >> p.lastTime;
    return in;
}
