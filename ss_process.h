/*
 * Shadowsocks Process Class
 *
 * Used to interact with the backend.
 *
 * Copyright 2014 William Wong <librehat@outlook.com>
 */
#include <QObject>
#include <QString>
#include <QProcess>
#include "profiles.h"

#ifndef SS_PROCESS_H
#define SS_PROCESS_H

class SS_Process : public QObject
{
    Q_OBJECT

public:
    SS_Process(QObject *parent = 0);
    ~SS_Process();
    void setapp(QString a);
    void start(QString server, QString pwd, QString s_port, QString l_port, QString method, QString timeout);
    void start(const QString &args);
    void start(SSProfile p);
    void stop();
    bool isRunning();

signals:
    void readReadyProcess(const QByteArray &o);
    void sigstart();
    void sigstop();

private:
    bool running;
    QString app_path;
    QProcess proc;

private slots:
    void autoemitreadReadyProcess();
    void started();
    void exited(int);
};

#endif // SS_PROCESS_H
