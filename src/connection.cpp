#include "connection.h"
#include "ssvalidator.h"

using namespace QSS;

Connection::Connection(const QString &_name, const QSS::Profile &profile, const bool _debug, QObject *parent) :
    QObject(parent),
    debug(_debug),
    name(_name)
{
    serverAddress = profile.server;
    serverPort = profile.server_port;
    localAddress = profile.local_address;
    localPort = profile.local_port;
    method = profile.method;
    password = profile.password;
    timeout = profile.timeout;

    controller = new Controller(true, this);

    connect(controller, &Controller::runningStateChanged, this, &Connection::stateChanged);
/*
    connect(controller, &Controller::runningStateChanged, [&](bool running){
        if (!running) {
            disconnect(controller, &Controller::info, logDlg, &LogDialog::append);
            disconnect(controller, &Controller::debug, logDlg, &LogDialog::append);
            disconnect(controller, &Controller::error, logDlg, &LogDialog::append);
        }
    });*/
}

Connection::~Connection()
{
}

const QString& Connection::getName() const
{
    return name;
}

QByteArray Connection::getURI() const
{
    QString ssurl = QString("%1:%2@%3:%4").arg(method.toLower()).arg(password).arg(serverAddress).arg(QString::number(serverPort));
    QByteArray ba = QByteArray(ssurl.toStdString().c_str()).toBase64();
    ba.prepend("ss://");
    return ba;
}

bool Connection::isValid() const
{
    if (serverAddress.isEmpty() || localAddress.isEmpty() || timeout < 1 || !SSValidator::validateMethod(method)) {
        return false;
    }
    else {
        return true;
    }
}

void Connection::start()
{
    /*
    connect(controller, logLevel == 0 ? &Controller::info :
                        logLevel == 1 ? &Controller::debug : &Controller::error,
            logDlg, &LogDialog::append);*/
    Profile profile;
    profile.server = serverAddress;
    profile.server_port = serverPort;
    profile.local_address = localAddress;
    profile.local_port = localPort;
    profile.method = method;
    profile.password = password;
    profile.timeout = timeout;
    controller->setup(profile);
    controller->start();
}

void Connection::stop()
{
    controller->stop();
}

