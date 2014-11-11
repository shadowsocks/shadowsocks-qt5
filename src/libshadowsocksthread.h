#ifndef LIBSHADOWSOCKSTHREAD_H
#define LIBSHADOWSOCKSTHREAD_H

#include <QThread>
#include <shadowsocks.h>
#include "ssprofile.h"

class LibshadowsocksThread : public QThread
{
    Q_OBJECT

public:
    LibshadowsocksThread(QObject *parent) : QThread(parent) {}
    void setProfile(SSProfile * const p);
    void run() Q_DECL_OVERRIDE;

private:
    profile_t profile;
    //use member variables to keep those data, otherwise the temporary QByteArray will be released by Qt.
    QByteArray remote_host;
    QByteArray method;
    QByteArray local_addr;
    QByteArray password;

    static QByteArray log_file;
};

#endif // LIBSHADOWSOCKSTHREAD_H
