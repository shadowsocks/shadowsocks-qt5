#include "controllerthread.h"

ControllerThread::ControllerThread(QObject *parent) :
    QThread(parent)
{}

QMutex ControllerThread::mutex;

void ControllerThread::setup(const QSS::Profile &_profile)
{
    profile = _profile;
}

void ControllerThread::run()
{
    mutex.lock();
    QSS::Controller controller(true);
    connect(&controller, &QSS::Controller::runningStateChanged, this, &ControllerThread::stateChanged);
    connect(&controller, &QSS::Controller::info, this, &ControllerThread::logAvailable);
    if (profile.debug) {
        connect(&controller, &QSS::Controller::debug, this, &ControllerThread::logAvailable);
    }
    connect(&controller, &QSS::Controller::newBytesReceived, this, &ControllerThread::newBytes);
    connect(&controller, &QSS::Controller::newBytesSent, this, &ControllerThread::newBytes);
    controller.setup(profile);
    bool started = controller.start();
    mutex.unlock();
    if (started) {
        exec();
    } else {
        emit failed();
    }
}
