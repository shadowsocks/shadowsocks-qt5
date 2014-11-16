#ifndef LIBSHADOWSOCKSTHREAD_H
#define LIBSHADOWSOCKSTHREAD_H

#include <QObject>
#include <QFile>
#include <QFileSystemWatcher>
#include <shadowsocks.h>
#include <pthread.h>
#include "ssprofile.h"

class LibshadowsocksThread : public QObject
{
    Q_OBJECT

public:
    LibshadowsocksThread(QObject *parent);
    void setProfile(SSProfile * const p, bool);
    //void run() Q_DECL_OVERRIDE;
    static void *threadStart(void *);

    profile_t libssprofile;
    static QByteArray log_file;

    void startThread();
    void stopThread();

signals:
    void started();
    void finished();
    void logReadyRead(const QByteArray &);

private:
    QFile *log;
    QFileSystemWatcher *logWatcher;
    //use member variables to keep those data, otherwise the temporary QByteArray will be released by Qt.
    QByteArray remote_host;
    QByteArray method;
    QByteArray local_addr;
    QByteArray password;

    pthread_t t;

private slots:
    void onLogFileChanged();
};

#endif // LIBSHADOWSOCKSTHREAD_H
