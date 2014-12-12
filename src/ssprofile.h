#ifndef SSPROFILE_H
#define SSPROFILE_H

#include <QString>
#include <QtShadowsocks>

class SSProfile
{
public:
    enum BackendType{LIBEV, NODEJS, GO, PYTHON, LIBQSS, UNKNOWN};
    SSProfile();
    QByteArray getSsUrl();
    bool isBackendMatchType();
    bool isValid() const;
    BackendType getBackendType() const;
    QString getBackend(bool relativePath = false);
    void setBackend(bool relativePath = false);
    void setBackend(const QString &a, bool relativePath = false);
    QSS::Profile getQSSProfile();

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
