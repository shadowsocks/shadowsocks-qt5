#include <QDir>
#include "libshadowsocksthread.h"

void LibshadowsocksThread::setProfile(SSProfile * const p)
{
    //TODO deep copy to avoid pitfalls
    profile.acl = NULL;
#ifdef __linux__
    profile.fast_open = p->fast_open ? 1 : 0,
#else
    profile.fast_open = 0,
#endif
    profile.log = QDir::tempPath().append("/libshadowsocks.log").toLatin1().data();
    profile.udp_relay = 1;
    profile.verbose = 0;

    profile.remote_host = p->server.toUtf8().data();
    profile.method = p->method.toUtf8().data();
    profile.local_addr = p->local_addr.toUtf8().data();
    profile.password = p->password.toUtf8().data();
    profile.remote_port = p->server_port.toInt();
    profile.local_port = p->local_port.toInt();
    profile.timeout = p->timeout.toInt();
}

void LibshadowsocksThread::run()
{
    start_ss_local_server(profile);
}
