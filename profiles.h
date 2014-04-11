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
    QString app;
    void setJSONFile(QString file);
    void setIndex(int);
    int getIndex();
    int count();
    QStringList getProfileList();
    SSProfile getProfile(int);
    SSProfile lastProfile();
    void revert();
    void addProfile(QString);
    void saveProfile(int, SSProfile);
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
