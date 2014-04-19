#include <QString>
#ifndef SSPROFILE_H
#define SSPROFILE_H
struct SSProfile {
    QString profileName;
    QString server;
    QString server_port;
    QString password;
    QString local_addr;
    QString local_port;
    QString method;
    QString timeout;

    SSProfile() : profileName(), server(), server_port("8388"), password(), local_addr("127.0.0.1"), local_port("1080"),  method("aes-256-cfb"), timeout("600") {}
};
#endif // SSPROFILE_H
