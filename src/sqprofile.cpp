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
}

SQProfile::SQProfile(const QSS::Profile &profile) : SQProfile()
{
    name = QString::fromStdString(profile.name());
    localAddress = QString::fromStdString(profile.localAddress());
    localPort = profile.localPort();
    serverPort = profile.serverPort();
    serverAddress = QString::fromStdString(profile.serverAddress());
    method = QString::fromStdString(profile.method()).toUpper();
    password = QString::fromStdString(profile.password());
    timeout = profile.timeout();
    httpMode = profile.httpProxy();
    debug = profile.debug();
}

SQProfile::SQProfile(const QString &uri)
{
    *this = SQProfile(QSS::Profile::fromUri(uri.toStdString()));
}

QSS::Profile SQProfile::toProfile() const
{
    QSS::Profile qssprofile;
    qssprofile.setName(name.toStdString());
    qssprofile.setServerAddress(serverAddress.toStdString());
    qssprofile.setServerPort(serverPort);
    qssprofile.setLocalAddress(localAddress.toStdString());
    qssprofile.setLocalPort(localPort);
    qssprofile.setMethod(method.toLower().toStdString());
    qssprofile.setPassword(password.toStdString());
    qssprofile.setTimeout(timeout);
    qssprofile.setHttpProxy(httpMode);
    if (debug) {
        qssprofile.enableDebug();
    } else {
        qssprofile.disableDebug();
    }
    return qssprofile;
}

QDataStream& operator << (QDataStream &out, const SQProfile &p)
{
    out << p.autoStart << p.debug << p.serverPort << p.localPort << p.name << p.serverAddress << p.localAddress << p.method << p.password << p.timeout << p.latency << p.currentUsage << p.totalUsage << p.lastTime << p.nextResetDate << p.httpMode;
    return out;
}

QDataStream& operator >> (QDataStream &in, SQProfile &p)
{
    in >> p.autoStart >> p.debug >> p.serverPort >> p.localPort >> p.name >> p.serverAddress >> p.localAddress >> p.method >> p.password >> p.timeout >> p.latency >> p.currentUsage >> p.totalUsage >> p.lastTime >> p.nextResetDate >> p.httpMode;
    return in;
}
