#include <QString>
#ifndef SSPROFILE_H
#define SSPROFILE_H
class SSProfile
{
public:
    SSProfile();
    void setBackend();
    void setBackend(const QString &a);
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
};
#endif // SSPROFILE_H
