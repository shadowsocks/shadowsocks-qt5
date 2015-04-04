/*
 * Connection Class
 * Copyright 2015 Symeon Huang <hzwhuang@gmail.com>
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QtShadowsocks>

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(const QString &_name, const QSS::Profile &profile, const bool _debug, QObject *parent = 0);
    Connection(Connection&&) = default;
    ~Connection();

    const QString& getName() const;
    QByteArray getURI() const;
    bool isValid() const;

signals:
    void stateChanged(bool started);

public slots:
    void start();
    void stop();

private:
    bool debug;
    quint16 serverPort;
    quint16 localPort;
    QString name;
    QString serverAddress;
    QString localAddress;
    QString method;
    QString password;
    int timeout;
    QSS::Controller *controller;

    friend class EditDialog;
};

#endif // CONNECTION_H
