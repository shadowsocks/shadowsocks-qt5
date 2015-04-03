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
    explicit Connection(const QString &_name, const QSS::Profile &_profile, const bool _debug, QObject *parent = 0);
    ~Connection();

    const QString& getName() const;
    QByteArray getURI() const;

signals:
    void stateChanged(bool started);

public slots:
    void start();
    void stop();

private:
    QString name;
    QSS::Controller *controller;
    QSS::Profile profile;
    bool debug;

    friend class EditDialog;
};

#endif // CONNECTION_H
