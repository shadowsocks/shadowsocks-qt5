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
    void setTranslucent(bool);
    bool isTranslucent();
    int count();
    QStringList getProfileList();
    inline SSProfile *profileAt(int i) { return &profileList[i]; }
    inline SSProfile *currentProfile() { return &profileList[m_index]; }
    inline SSProfile *lastProfile() { return &profileList.last(); }
    void revert();
    void addProfile(const QString &);
    void addProfileFromSSURI(const QString &, QString);
    void deleteProfile(int);
    void saveProfileToJSON();

private:
    int m_index;
    bool debugLog;
    bool autoHide;
    bool autoStart;
    bool translucent;
    QList<SSProfile> profileList;
    QString m_file;
    QJsonDocument JSONDoc;
    QJsonObject JSONObj;
};

#endif // PROFILES_H
