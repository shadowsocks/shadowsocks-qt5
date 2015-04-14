#include "controllerthread.h"

ControllerThread::ControllerThread(QObject *parent) :
    QThread(parent)
{}

void ControllerThread::setup(bool _debug, const QSS::Profile &_profile)
{
    debug = _debug;
    profile = _profile;
}

void ControllerThread::run()
{
    QSS::Controller controller(profile, true);
    connect(&controller, &QSS::Controller::runningStateChanged, this, &ControllerThread::stateChanged);
    connect(&controller, debug ? &QSS::Controller::debug : &QSS::Controller::error, this, &ControllerThread::logAvailable);
    connect(&controller, &QSS::Controller::newBytesReceived, this, &ControllerThread::newBytesRead);
    connect(&controller, &QSS::Controller::newBytesSent, this, &ControllerThread::newBytesSent);

    controller.start();
    exec();
}
