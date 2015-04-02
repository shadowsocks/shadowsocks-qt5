/*
 * Connection Class
 * Copyright 2015 Symeon Huang <hzwhuang@gmail.com>
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QtShadowsocks>
#include "editdialog.h"
#include "logdialog.h"

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(const QString &_name, const QSS::Profile &_profile, const int &_loglevel, QObject *parent = 0);
    ~Connection();

signals:
    void stateChanged(bool started);

public slots:
    void start();
    void stop();
    void edit();
    void shareDialog();
    void logDialog();

private:
    QString name;
    QSS::Controller *controller;
    QSS::Profile profile;
    int logLevel;
    EditDialog *editDlg;
    LogDialog *logDlg;
};

#endif // CONNECTION_H
