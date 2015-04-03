#include "connection.h"
#include "sharedialog.h"

using namespace QSS;

Connection::Connection(const QString &_name, const QSS::Profile &_profile, const bool _debug, QObject *parent) :
    QObject(parent),
    name(_name),
    profile(_profile),
    debug(_debug)
{
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
    QString ssurl = QString("%1:%2@%3:%4").arg(profile.method.toLower()).arg(profile.password).arg(profile.server).arg(QString::number(profile.server_port));
    QByteArray ba = QByteArray(ssurl.toStdString().c_str()).toBase64();
    ba.prepend("ss://");
    return ba;
}

void Connection::start()
{
    /*
    connect(controller, logLevel == 0 ? &Controller::info :
                        logLevel == 1 ? &Controller::debug : &Controller::error,
            logDlg, &LogDialog::append);*/
    controller->setup(profile);
    controller->start();
}

void Connection::stop()
{
    controller->stop();
}

