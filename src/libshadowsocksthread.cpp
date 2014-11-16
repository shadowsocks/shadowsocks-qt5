#include <QDir>
#include <signal.h>
#include "libshadowsocksthread.h"

LibshadowsocksThread::LibshadowsocksThread(QObject *parent) :
    QObject(parent)
{
    log = new QFile(this);
    log->setFileName(QString::fromLocal8Bit(log_file));
}

QByteArray LibshadowsocksThread::log_file = QDir::tempPath().append("/libshadowsocks.log").toLocal8Bit();

void LibshadowsocksThread::setProfile(SSProfile * const p, bool debug)
{
    libssprofile.acl = NULL;
#ifdef __linux__
    libssprofile.fast_open = p->fast_open ? 1 : 0,
#else
    libssprofile.fast_open = 0,
#endif
    libssprofile.log = log_file.data();
    libssprofile.udp_relay = 1;
    libssprofile.verbose = debug ? 1 : 0;

    libssprofile.remote_port = p->server_port.toInt();
    libssprofile.local_port = p->local_port.toInt();
    libssprofile.timeout = p->timeout.toInt();

    //save temporary QByteArray at first
    remote_host = p->server.toLocal8Bit();
    method = p->method.toLower().toLocal8Bit();
    local_addr = p->local_addr.toLocal8Bit();
    password = p->password.toLocal8Bit();

    libssprofile.remote_host = remote_host.data();
    libssprofile.method = method.data();
    libssprofile.local_addr = local_addr.data();
    libssprofile.password = password.data();
}

void *LibshadowsocksThread::threadStart(void * p)
{
    int result = start_ss_local_server(*static_cast<profile_t *>(p));
    pthread_exit(&result);
}

void LibshadowsocksThread::startThread()
{
    log->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Unbuffered);
    log->write('\0');
    logWatcher = new QFileSystemWatcher(this);
    logWatcher->addPath(QString::fromLocal8Bit(log_file));
    connect(logWatcher, &QFileSystemWatcher::fileChanged, this, &LibshadowsocksThread::onLogFileChanged);
    if (pthread_create(&t, NULL, threadStart, static_cast<void *>(&libssprofile)) == 0) {
        emit started();
    }
}

void LibshadowsocksThread::stopThread()
{
    if (logWatcher != NULL && pthread_kill(t, SIGTERM) == 0) {
        log->close();
        logWatcher->deleteLater();
        logWatcher = NULL;
        emit finished();
    }
}

void LibshadowsocksThread::onLogFileChanged()
{
    emit logReadyRead(log->readAll());
}
