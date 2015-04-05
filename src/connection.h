/*
 * Connection Class
 * Copyright 2015 Symeon Huang <hzwhuang@gmail.com>
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QDateTime>
#include <QtShadowsocks>

struct SQProfile
{
    bool autoStart;
    bool debug;
    quint16 serverPort;
    quint16 localPort;
    QString name;
    QString serverAddress;
    QString localAddress;
    QString method;
    QString password;
    int timeout;
    int lag;
    QDateTime lastTime;
};
Q_DECLARE_METATYPE(SQProfile)

QDataStream& operator << (QDataStream &out, const SQProfile &p);
QDataStream& operator >> (QDataStream &in, SQProfile &p);

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
