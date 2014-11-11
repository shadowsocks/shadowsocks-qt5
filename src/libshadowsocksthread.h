#ifndef LIBSHADOWSOCKSTHREAD_H
#define LIBSHADOWSOCKSTHREAD_H

#include <QThread>
#include <shadowsocks.h>
#include "ssprofile.h"

class LibshadowsocksThread : public QThread
{
    Q_OBJECT

public:
    LibshadowsocksThread(QObject *parent);
    void setProfile(SSProfile * const p);
    void run() Q_DECL_OVERRIDE;

    static QByteArray log_file;

private:
    profile_t profile;
    //use member variables to keep those data, otherwise the temporary QByteArray will be released by Qt.
    QByteArray remote_host;
    QByteArray method;
    QByteArray local_addr;
    QByteArray password;
};

#endif // LIBSHADOWSOCKSTHREAD_H
