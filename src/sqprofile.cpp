#include "sqprofile.h"

SQProfile::SQProfile()
{
    autoStart = false;
    debug = false;
    serverPort = 8388;
    localPort = 1080;
    name = QObject::tr("Unnamed Profile");
    localAddress = QString("127.0.0.1");
    method = QString("RC4-MD5");
    timeout = 600;
    latency = LATENCY_UNKNOWN;
    currentUsage = 0;
    totalUsage = 0;
    QDate currentDate = QDate::currentDate();
    nextResetDate = QDate(currentDate.year(), currentDate.month() + 1, 1);
    httpMode = false;
    onetimeAuth = false;
}

SQProfile::SQProfile(const QSS::Profile &profile) : SQProfile()
{
    localAddress = profile.local_address;
    localPort = profile.local_port;
    serverPort = profile.server_port;
    serverAddress = profile.server;
    method = profile.method;
    password = profile.password;
    timeout = profile.timeout;
    httpMode = profile.http_proxy;
    debug = profile.debug;
    onetimeAuth = profile.auth;
}

SQProfile::SQProfile(const QString &uri)
{
    *this = SQProfile(QSS::Profile(uri.toLocal8Bit()));
}

QSS::Profile SQProfile::toProfile() const
{
    QSS::Profile qssprofile;
    qssprofile.server = serverAddress;
    qssprofile.server_port = serverPort;
    qssprofile.local_address = localAddress;
    qssprofile.local_port = localPort;
    qssprofile.method = method.toLower();
    qssprofile.password = password;
    qssprofile.timeout = timeout;
    qssprofile.http_proxy = httpMode;
    qssprofile.debug = debug;
    qssprofile.auth = onetimeAuth;
    return qssprofile;
}

QDataStream& operator << (QDataStream &out, const SQProfile &p)
{
    out << p.autoStart << p.debug << p.serverPort << p.localPort << p.name << p.serverAddress << p.localAddress << p.method << p.password << p.timeout << p.latency << p.currentUsage << p.totalUsage << p.lastTime << p.nextResetDate << p.httpMode << p.onetimeAuth;
    return out;
}

QDataStream& operator >> (QDataStream &in, SQProfile &p)
{
    in >> p.autoStart >> p.debug >> p.serverPort >> p.localPort >> p.name >> p.serverAddress >> p.localAddress >> p.method >> p.password >> p.timeout >> p.latency >> p.currentUsage >> p.totalUsage >> p.lastTime >> p.nextResetDate >> p.httpMode >> p.onetimeAuth;
    return in;
}
