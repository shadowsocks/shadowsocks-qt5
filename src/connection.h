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
    const QString& getName() const;
    QByteArray getURI() const;
    bool isValid() const;

signals:
    void stateChanged(bool started);

public slots:
    void start();
    void stop();

private:
    SQProfile profile;
    QSS::Controller *controller;

    friend class EditDialog;
    friend class ConfigHelper;
};
Q_DECLARE_METATYPE(Connection*)

#endif // CONNECTION_H
