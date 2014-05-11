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
#include "ssprofile.h"

#ifndef PROFILES_H
#define PROFILES_H

class Profiles
{
public:
    Profiles(const QString &file);
    ~Profiles();
    void setJSONFile(const QString &);
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
    inline SSProfile &rProfileAt(int i) { return profileList[i]; }
    inline SSProfile &rCurrentProfile() { return profileList[m_index]; }
    inline SSProfile &rLastProfile() { return profileList.last(); }
    void revert();
    void addProfile(const QString &);
    void addProfileFromSSURI(const QString &, QString);
    void saveProfile(int, SSProfile &);
    void deleteProfile(int);
    void saveProfileToJSON();
    bool isValidate(const SSProfile &);//check backend as well as profile

private:
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
