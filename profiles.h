#include <QFile>
#include <QString>
#include <QStringList>
#include <QList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#ifndef PROFILES_H
#define PROFILES_H

typedef struct {
    QString server;
    QString server_port;
    QString password;
    QString local_port;
    QString method;
    QString timeout;
} SSProfile;

class Profiles
{
public:
    Profiles(QString file = QString());
    ~Profiles();
    QString app;
    void setJSONFile(QString file);
    void setIndex(int index);
    int getIndex();
    int count();
    QStringList getserverList();
    SSProfile getProfile(int index);
    SSProfile lastProfile();
    /*
    QString getSever(int index);
    QString getPassword(int index);
    QString getServer_port(int index);
    QString getLocal_port(int index);
    QString getMethod(int index);
    QString getTimeout(int index);
    */
    void addProfile(QString s);
    void saveProfile(int index, SSProfile p);
    //void saveProfile(int index, QString s, QString sport, QString pwd, QString lport, QString m, QString t);
    void deleteProfile(int index);
    void saveProfileToJSON();
private:
    int m_index;
    QList<SSProfile> profileList;
    QString m_file;
    QJsonDocument JSONDoc;
    QJsonObject JSONObj;
    QJsonArray CONFArray;
};

#endif // PROFILES_H
