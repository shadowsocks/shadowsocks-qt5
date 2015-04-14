#ifndef CONTROLLERTHREAD_H
#define CONTROLLERTHREAD_H

#include <QThread>
#include <QtShadowsocks>

class ControllerThread : public QThread
{
    Q_OBJECT
public:
    ControllerThread(QObject *parent);
    void setup(bool _debug, const QSS::Profile &_profile);
    void run() Q_DECL_OVERRIDE;

signals:
    void logAvailable(const QString &);
    void stateChanged(bool);
    void newBytesRead(quint64);
    void newBytesSent(quint64);

private:
    bool debug;
    QSS::Profile profile;
};

#endif // CONTROLLERTHREAD_H
