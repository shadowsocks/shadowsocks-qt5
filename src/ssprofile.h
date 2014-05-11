#include <QString>
#ifndef SSPROFILE_H
#define SSPROFILE_H
class SSProfile
{
public:
    SSProfile() : backend(), type("Shadowsocks-libev"),  profileName(), server(), server_port("8388"), password(), local_addr("127.0.0.1"), local_port("1080"),  method("aes-256-cfb"), timeout("600") {}

    void setBackend();
    void setBackend(const QString &a);
    int getBackendTypeID();
    bool isBackendMatchType();

    QString backend;
    QString type;
    QString profileName;
    QString server;
    QString server_port;
    QString password;
    QString local_addr;
    QString local_port;
    QString method;
    QString timeout;
};
#endif // SSPROFILE_H
