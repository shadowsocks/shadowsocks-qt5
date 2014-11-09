#ifndef SSPROFILE_H
#define SSPROFILE_H

#include <QString>
#include <shadowsocks.h>

class SSProfile
{
public:
    enum BackendType{LIBEV, NODEJS, GO, PYTHON, LIBSHADOWSOCKS, UNKNOWN};
    SSProfile();
    QByteArray getSsUrl();
    bool isBackendMatchType();
    bool isValid() const;
    BackendType getBackendType() const;
    QString getBackend(bool relativePath = false);
    void setBackend(bool relativePath = false);
    void setBackend(const QString &a, bool relativePath = false);
    profile_t getLibshadowsocksProfile() const;//return the profile in the format defined in shadowsocks.h (a C struct)

    QString backend;
    QString custom_arg;
    bool fast_open;
    QString local_addr;
    QString local_port;
    QString method;
    QString password;
    QString profileName;
    QString server;
    QString server_port;
    QString timeout;
    QString type;
};
#endif // SSPROFILE_H
