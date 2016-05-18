#include "connection.h"
#include "ssvalidator.h"
#include <QHostInfo>
#include <QHostAddress>

Connection::Connection(QObject *parent) :
    QObject(parent),
    controller(nullptr),
    running(false)
{}

Connection::Connection(const SQProfile &_profile, QObject *parent) :
    Connection(parent)
{
    profile = _profile;
}

Connection::Connection(QString uri, QObject *parent) :
    Connection(parent)
{
    profile = SQProfile(uri);
}

Connection::~Connection()
{
    stop();
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
    return profile.toProfile().toURI();
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
    QHostAddress serverAddr(profile.serverAddress);
    if (serverAddr.isNull()) {
        QHostInfo::lookupHost(profile.serverAddress, this, SLOT(onServerAddressLookedUp(QHostInfo)));
    } else {
        testAddressLatency(serverAddr);
    }
}

void Connection::start()
{
    profile.lastTime = QDateTime::currentDateTime();
    //perform a latency test if the latency is unknown
    if (profile.latency == SQProfile::LATENCY_UNKNOWN) {
        latencyTest();
    }

    QSS::Profile qssprofile = profile.toProfile();

    if (controller) {
        controller->deleteLater();
    }
    controller = new QSS::Controller(true, false, this);
    connect(controller, &QSS::Controller::runningStateChanged, [&](bool run){
        running = run;
        emit stateChanged(run);
    });
    connect(controller, &QSS::Controller::tcpLatencyAvailable, this, &Connection::onLatencyAvailable);
    connect(controller, &QSS::Controller::newBytesReceived, this, &Connection::onNewBytesTransmitted);
    connect(controller, &QSS::Controller::newBytesSent, this, &Connection::onNewBytesTransmitted);
    connect(controller, &QSS::Controller::info, this, &Connection::onNewLog);
    if (profile.debug) {
        connect(controller, &QSS::Controller::debug, this, &Connection::onNewLog);
    }

    controller->setup(qssprofile);
    if (!controller->start()) {
        emit startFailed();
    }
}

void Connection::stop()
{
    if (running) {
        if (controller) {
            controller->stop();
        }
    }
}

void Connection::testAddressLatency(const QHostAddress &addr)
{
    QSS::AddressTester *addrTester = new QSS::AddressTester(addr, profile.serverPort, this);
    connect(addrTester, &QSS::AddressTester::connectivityTestFinished, this, &Connection::onConnectivityTestFinished, Qt::QueuedConnection);
    connect(addrTester, &QSS::AddressTester::lagTestFinished, this, &Connection::onLatencyAvailable, Qt::QueuedConnection);
    addrTester->startConnectivityTest(profile.method, profile.password, profile.onetimeAuth);
}

void Connection::onNewBytesTransmitted(const quint64 &b)
{
    profile.currentUsage += b;
    profile.totalUsage += b;
    emit dataUsageChanged(profile.currentUsage, profile.totalUsage);
}

void Connection::onNewLog(const QString &str)
{
    if(!log.endsWith('\n') && !log.isEmpty()) {
        log.append('\n');
    }
    log.append(str);
    emit newLogAvailable(str);
}

void Connection::onServerAddressLookedUp(const QHostInfo &host)
{
    if (host.error() == QHostInfo::NoError) {
        testAddressLatency(host.addresses().first());
    } else {
        onLatencyAvailable(SQProfile::LATENCY_ERROR);
    }
}

void Connection::onLatencyAvailable(const int latency)
{
    profile.latency = latency;
    emit latencyAvailable(latency);
}

void Connection::onConnectivityTestFinished(bool con)
{
    QSS::AddressTester* tester = qobject_cast<QSS::AddressTester*>(sender());
    if (!con) {
        disconnect(tester, &QSS::AddressTester::lagTestFinished, this, &Connection::onLatencyAvailable);
        this->onLatencyAvailable(SQProfile::LATENCY_ERROR);
        onNewLog(QStringLiteral("Internet connectivity test failed. Please check the connection's profile and your firewall settings."));
    }
    tester->deleteLater();
}
