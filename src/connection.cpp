#include "connection.h"
#include "ssvalidator.h"

QDataStream& operator << (QDataStream &out, const SQProfile &p)
{
    out << p.debug << p.serverPort << p.localPort << p.name << p.serverAddress << p.localAddress << p.method << p.password << p.timeout << p.lag << p.lastTime;
    return out;
}

QDataStream& operator >> (QDataStream &in, SQProfile &p)
{
    in >> p.debug >> p.serverPort >> p.localPort >> p.name >> p.serverAddress >> p.localAddress >> p.method >> p.password >> p.timeout >> p.lag >> p.lastTime;
    return in;
}

Connection::Connection(QObject *parent) : QObject(parent) {}

Connection::Connection(const SQProfile &_profile, QObject *parent) :
    Connection(parent)
{
    profile = _profile;
}

Connection::Connection(QString uri, QObject *parent) :
    Connection(parent)
{
    uri.remove(0, 5);//remove the prefix "ss://" from uri
    QStringList resultList = QString(QByteArray::fromBase64(QByteArray(uri.toStdString().c_str()))).split(':');
    profile.method = resultList.takeFirst().toUpper();
    profile.serverPort = resultList.takeLast().toShort();
    QStringList ser = resultList.join(':').split('@');//there are lots of ':' in IPv6 address
    profile.serverAddress = ser.takeLast();
    profile.password = ser.join('@');//incase there is a '@' in password
}

Connection::~Connection()
{
}

const SQProfile& Connection::getProfile() const
{
    return profile;
}

const QString& Connection::getName() const
{
    return profile.name;
}

QByteArray Connection::getURI() const
{
    QString ssurl = QString("%1:%2@%3:%4").arg(profile.method.toLower()).arg(profile.password).arg(profile.serverAddress).arg(QString::number(profile.serverPort));
    QByteArray ba = QByteArray(ssurl.toStdString().c_str()).toBase64();
    ba.prepend("ss://");
    return ba;
}

bool Connection::isValid() const
{
    if (profile.serverAddress.isEmpty() || profile.localAddress.isEmpty() || profile.timeout < 1 || !SSValidator::validateMethod(profile.method)) {
        return false;
    }
    else {
        return true;
    }
}

void Connection::start()
{
    QSS::Profile qssprofile;
    qssprofile.server = profile.serverAddress;
    qssprofile.server_port = profile.serverPort;
    qssprofile.local_address = profile.localAddress;
    qssprofile.local_port = profile.localPort;
    qssprofile.method = profile.method;
    qssprofile.password = profile.password;
    qssprofile.timeout = profile.timeout;
    controller->setup(qssprofile);
    controller->start();
    profile.lastTime = QDateTime::currentDateTime();
}

void Connection::stop()
{
    controller->stop();
}

