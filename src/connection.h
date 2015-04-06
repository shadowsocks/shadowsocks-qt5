/*
 * Connection Class
 * Copyright 2015 Symeon Huang <hzwhuang@gmail.com>
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QtShadowsocks>
#include "sqprofile.h"

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

signals:
    void stateChanged(bool started);
    void newLogAvailable(const QString &);

public slots:
    void start();
    void stop();

private:
    SQProfile profile;
    QSS::Controller *controller;
    bool running;
    QString log;

    friend class EditDialog;
    friend class ConfigHelper;

private slots:
    void onNewLog(const QString &);
};
Q_DECLARE_METATYPE(Connection*)

#endif // CONNECTION_H
