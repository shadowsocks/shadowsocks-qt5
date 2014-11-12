#include <QDir>
#include <signal.h>
#include "libshadowsocksthread.h"

LibshadowsocksThread::LibshadowsocksThread(QObject *parent) :
    QObject(parent)
{
}

QByteArray LibshadowsocksThread::log_file = QDir::tempPath().append("/libshadowsocks.log").toUtf8();

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
    remote_host = p->server.toUtf8();
    method = p->method.toLower().toUtf8();
    local_addr = p->local_addr.toUtf8();
    password = p->password.toUtf8();

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

bool LibshadowsocksThread::startThread()
{
    if (pthread_create(&t, NULL, threadStart, static_cast<void *>(&libssprofile)) == 0) {
        emit started();
        return true;
    }
    else {
        return false;
    }
}

bool LibshadowsocksThread::stopThread()
{
    if (pthread_kill(t, SIGTERM) == 0) {
        emit finished();
        return true;
    }
    else {
        return false;
    }
}
