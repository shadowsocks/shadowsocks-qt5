/*
 * Copyright 2014 William Wong <librehat@outlook.com>
 */
#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <QString>
#include <QStringList>
#include <QList>
#include "ssprofile.h"

class Configuration
{
public:
    Configuration(const QString &file);
    ~Configuration();
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
    bool isTFOAvailable() const;
    int count();
    QStringList getProfileList();
    inline SSProfile *profileAt(int i) { return &profileList[i]; }
    inline SSProfile *currentProfile() { return &profileList[m_index]; }
    inline SSProfile *lastProfile() { return &profileList.last(); }
    void revert();
    void addProfile(const QString &);
    void addProfileFromSSURI(const QString &, QString);
    void deleteProfile(int);
    void save();

private:
    int m_index;
    bool debugLog;
    bool autoHide;
    bool autoStart;
    bool translucent;
    QList<SSProfile> profileList;
    QString m_file;
    static bool tfo_available;
};

#endif // CONFIGURATION_H
