#include "connection.h"
#include "sharedialog.h"

using namespace QSS;

Connection::Connection(const QString &_name, const QSS::Profile &_profile, const int &_loglevel, QObject *parent) :
    QObject(parent),
    name(_name),
    profile(_profile),
    logLevel(_loglevel)
{
    controller = new Controller(true, this);
    logDlg = new LogDialog;

    connect(controller, &Controller::runningStateChanged, this, &Connection::stateChanged);

    connect(controller, &Controller::runningStateChanged, [&](bool running){
        if (!running) {
            disconnect(controller, &Controller::info, logDlg, &LogDialog::append);
            disconnect(controller, &Controller::debug, logDlg, &LogDialog::append);
            disconnect(controller, &Controller::error, logDlg, &LogDialog::append);
        }
    });
}

Connection::~Connection()
{
    logDlg->deleteLater();
}

void Connection::start()
{
    logDlg->clear();
    connect(controller, logLevel == 0 ? &Controller::info :
                        logLevel == 1 ? &Controller::debug : &Controller::error,
            logDlg, &LogDialog::append);
    controller->setup(profile);
    controller->start();
}

void Connection::stop()
{
    controller->stop();
}

void Connection::edit()
{
    //TODO
}

void Connection::shareDialog()
{
    QString ssurl = QString("%1:%2@%3:%4").arg(profile.method.toLower()).arg(profile.password).arg(profile.server).arg(QString::number(profile.server_port));
    QByteArray ba = QByteArray(ssurl.toStdString().c_str()).toBase64();
    ba.prepend("ss://");
    ShareDialog dlg(ba);
    dlg.exec();
}

void Connection::logDialog()
{
    logDlg->show();
}
