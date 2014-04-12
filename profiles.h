/*
 * Multi-Profile (so just call it Profiles) class
 * - read from file
 * - manage profiles
 * - write to file
 *
 * Copyright 2014 William Wong <librehat@outlook.com>
 */
#include <QFile>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#ifndef PROFILES_H
#define PROFILES_H

typedef struct {
    QString profileName;
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
    void setBackend(const QString&);
    QString getBackend();
    void setJSONFile(const QString&);
    void setIndex(int);
    int getIndex();
    void setAutoStart(bool);
    bool isAutoStart();
    void setAutoHide(bool);
    bool isAutoHide();
    void setDebug(bool);
    bool isDebug();
    int count();
    QStringList getProfileList();
    SSProfile getProfile(int);
    SSProfile lastProfile();
    void revert();
    void addProfile(const QString&);
    void saveProfile(int, SSProfile&);
    //void saveAllProfile();
    void deleteProfile(int);
    void saveProfileToJSON();
    bool isValidate(SSProfile&);//check backend as well as profile

private:
    QString backend;
    int m_index;
    bool debugLog;
    bool autoHide;
    bool autoStart;
    QList<SSProfile> profileList;
    QString m_file;
    QJsonDocument JSONDoc;
    QJsonObject JSONObj;
    QJsonArray CONFArray;
};

#endif // PROFILES_H
