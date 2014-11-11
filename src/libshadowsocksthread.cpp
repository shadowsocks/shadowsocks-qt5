#include <QDir>
#include "libshadowsocksthread.h"

LibshadowsocksThread::LibshadowsocksThread(QObject *parent) :
    QThread(parent)
{
    this->setTerminationEnabled(true);
}

QByteArray LibshadowsocksThread::log_file = QDir::tempPath().append("/libshadowsocks.log").toUtf8();

void LibshadowsocksThread::setProfile(SSProfile * const p, bool debug)
{
    profile.acl = NULL;
#ifdef __linux__
    profile.fast_open = p->fast_open ? 1 : 0,
#else
    profile.fast_open = 0,
#endif
    profile.log = log_file.data();
    profile.udp_relay = 1;
    profile.verbose = debug ? 1 : 0;

    profile.remote_port = p->server_port.toInt();
    profile.local_port = p->local_port.toInt();
    profile.timeout = p->timeout.toInt();

    //save temporary QByteArray at first
    remote_host = p->server.toUtf8();
    method = p->method.toLower().toUtf8();
    local_addr = p->local_addr.toUtf8();
    password = p->password.toUtf8();

    profile.remote_host = remote_host.data();
    profile.method = method.data();
    profile.local_addr = local_addr.data();
    profile.password = password.data();
}

void LibshadowsocksThread::run()
{
    start_ss_local_server(profile);
    exec();
}
