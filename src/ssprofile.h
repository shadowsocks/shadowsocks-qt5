#ifndef SSPROFILE_H
#define SSPROFILE_H

#include <QString>

class SSProfile
{
public:
    SSProfile();
    void setBackend(bool relativePath = false);
    void setBackend(const QString &a, bool relativePath = false);
    QString getBackend();
    int getBackendTypeID();
    bool isBackendMatchType();
    bool isValid() const;

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
    bool fast_open;
};
#endif // SSPROFILE_H
