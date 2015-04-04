#include "connection.h"
#include "ssvalidator.h"

using namespace QSS;

QDataStream& operator << (QDataStream &out, const SQProfile &p)
{
    out << p.debug << p.serverPort << p.localPort << p.name << p.serverAddress << p.localAddress << p.method << p.password << p.timeout;
    return out;
}

QDataStream& operator >> (QDataStream &in, SQProfile &p)
{
    in >> p.debug >> p.serverPort >> p.localPort >> p.name >> p.serverAddress >> p.localAddress >> p.method >> p.password >> p.timeout;
    return in;
}

Connection::Connection(const SQProfile &_profile, QObject *parent) :
    QObject(parent),
    profile(_profile)
{
    controller = new Controller(true, this);

    connect(controller, &Controller::runningStateChanged, this, &Connection::stateChanged);
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
    Profile qssprofile;
    qssprofile.server = profile.serverAddress;
    qssprofile.server_port = profile.serverPort;
    qssprofile.local_address = profile.localAddress;
    qssprofile.local_port = profile.localPort;
    qssprofile.method = profile.method;
    qssprofile.password = profile.password;
    qssprofile.timeout = profile.timeout;
    controller->setup(qssprofile);
    controller->start();
}

void Connection::stop()
{
    controller->stop();
}

