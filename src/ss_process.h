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
#include <QtShadowsocks>
#include "ssprofile.h"

class SS_Process : public QObject
{
    Q_OBJECT

public:
    SS_Process(QObject *parent = 0);
    ~SS_Process();
    void start(SSProfile * const, bool debug);
    void stop();

signals:
    void processRead(const QByteArray &o);
    void processStarted();
    void processStopped();

private:
    bool libQSS;
    QSS::Controller *qssController;
    SSProfile::BackendType backendType;
    QString app_path;
    QProcess proc;

    void startQSS(SSProfile * const, bool);
    void start(const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, bool debug = false, bool tfo = false);
    void start(QString &args);

private slots:
    void onProcessReadyRead();
    void onQSSInfoReady(const QString &);
    void onStarted();
    void onExited(int);
};

#endif // SS_PROCESS_H
