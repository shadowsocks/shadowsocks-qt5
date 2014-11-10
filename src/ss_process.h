/*
 * Shadowsocks Process Class
 *
 * Used to interact with the backend.
 *
 * Copyright 2014 William Wong <librehat@outlook.com>
 */
#ifndef SS_PROCESS_H
#define SS_PROCESS_H
#include <QObject>
#include <QString>
#include <QProcess>
#include <QThread>
#include "ssprofile.h"

class LibshadowsocksThread : public QThread
{
    Q_OBJECT

public:
    LibshadowsocksThread(QObject *parent) : QThread(parent) {}
    void setProfile(const profile_t &p) { profile = p; }
    void run() Q_DECL_OVERRIDE {
        start_ss_local_server(profile);
    }

private:
    profile_t profile;
};

class SS_Process : public QObject
{
    Q_OBJECT

public:
    SS_Process(QObject *parent = 0);
    ~SS_Process();
    void start(SSProfile * const, bool debug = false);
    void stop();
    bool isRunning();

signals:
    void processRead(const QByteArray &o);
    void processStarted();
    void processStopped();

private:
    bool running;
    bool libshadowsocks;
    LibshadowsocksThread *libssThread;
    SSProfile::BackendType backendType;
    QString app_path;
    QProcess proc;

    void startLibshadowsocks(const profile_t &);
    void start(const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, bool debug = false, bool tfo = false);
    void start(QString &args);

private slots:
    void onProcessReadyRead();
    void onStarted();
    void onExited(int);
};

#endif // SS_PROCESS_H
