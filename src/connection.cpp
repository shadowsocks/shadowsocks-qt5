#include "connection.h"
#include "ssvalidator.h"

Connection::Connection(QObject *parent) :
    QObject(parent),
    running(false)
{
    controller = new QSS::Controller(true, this);
    connect(controller, &QSS::Controller::runningStateChanged, [&](bool run){
        running = run;
        emit stateChanged(run);
    });
    connect(controller, &QSS::Controller::bytesReceivedChanged, [&](const quint64 &b) {
        emit bytesReadChanged(b);
        profile.bytesRead = b;
    });
    connect(controller, &QSS::Controller::bytesSentChanged, [&](const quint64 &b) {
        emit bytesSentChanged(b);
        profile.bytesSent = b;
    });
}

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

const QString& Connection::getLog() const
{
    return log;
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

const bool &Connection::isRunning() const
{
    return running;
}

void Connection::latencyTest()
{
    QSS::Address server(profile.serverAddress, profile.serverPort);
    profile.lag = server.ping();
}

bool Connection::start()
{
    disconnect(controller, &QSS::Controller::debug, this, &Connection::onNewLog);
    disconnect(controller, &QSS::Controller::error, this, &Connection::onNewLog);
    connect(controller, profile.debug ? &QSS::Controller::debug : &QSS::Controller::error, this, &Connection::onNewLog);

    profile.lastTime = QDateTime::currentDateTime();
    latencyTest();//perform a latency test automatically when start() is called

    QSS::Profile qssprofile;
    qssprofile.server = profile.serverAddress;
    qssprofile.server_port = profile.serverPort;
    qssprofile.local_address = profile.localAddress;
    qssprofile.local_port = profile.localPort;
    qssprofile.method = profile.method;
    qssprofile.password = profile.password;
    qssprofile.timeout = profile.timeout;
    controller->setup(qssprofile);
    return controller->start();
}

void Connection::stop()
{
    controller->stop();
}

void Connection::onNewLog(const QString &str)
{
    if(!log.endsWith('\n') && !log.isEmpty()) {
        log.append('\n');
    }
    log.append(str);
    emit newLogAvailable(str);
}
