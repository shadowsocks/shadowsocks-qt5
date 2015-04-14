/*
 * Connection Class
 * Copyright 2015 Symeon Huang <hzwhuang@gmail.com>
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QtShadowsocks>
#include "sqprofile.h"
#include "controllerthread.h"

class Connection : public QObject
{
    Q_OBJECT
public:
    Connection(QObject *parent = 0);
    Connection(const SQProfile &_profile, QObject *parent = 0);
    Connection(QString uri, QObject *parent = 0);
    ~Connection();

    const SQProfile &getProfile() const;
    const QString &getName() const;
    const QString &getLog() const;
    QByteArray getURI() const;
    bool isValid() const;
    const bool &isRunning() const;
    void latencyTest();

signals:
    void stateChanged(bool started);
    void pingFinished(const int);
    void newLogAvailable(const QString &);
    void bytesReadChanged(const quint64 &);
    void bytesSentChanged(const quint64 &);

public slots:
    void start();
    void stop();

private:
    ControllerThread *controllerThread;
    SQProfile profile;
    bool running;
    QString log;

    friend class EditDialog;
    friend class ConfigHelper;
    friend class StatusDialog;
    friend class LogDialog;

private slots:
    void onNewLog(const QString &);
    void onLagTestFinished(int);
};
Q_DECLARE_METATYPE(Connection*)

#endif // CONNECTION_H
