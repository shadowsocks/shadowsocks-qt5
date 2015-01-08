/*
 * Copyright 2014-2015 Symeon Huang <hzwhuang@gmail.com>
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

    inline bool isAutoHide() const { return autoHide; }
    inline bool isAutoStart() const { return autoStart; }
    inline bool isDebug() const { return debugLog; }
    inline bool isRelativePath() const { return relativePath; }
    inline bool isTFOAvailable() const { return tfo_available; }
    inline bool isTranslucent() const { return translucent; }
    inline bool isUseSystray() const { return useSystray; }
    inline int count() const { return profileList.count(); }
    inline int getIndex() const { return m_index; }
    inline SSProfile *currentProfile() { return &profileList[m_index]; }
    inline SSProfile *lastProfile() { return &profileList.last(); }
    inline SSProfile *profileAt(int i) { return &profileList[i]; }
    inline void deleteProfile(int index) { profileList.removeAt(index); }
    inline void revert() { setJSONFile(m_file); }
    inline void setAutoHide(bool b) { autoHide = b; }
    inline void setAutoStart(bool b) { autoStart = b; }
    inline void setDebug(bool b) { debugLog = b; }
    inline void setIndex(int i) { m_index = i; }
    inline void setRelativePath(bool b) { relativePath = b; }
    inline void setTranslucent(bool b) { translucent = b; }
    inline void setUseSystray(bool b) { useSystray = b; }
    QStringList getProfileList();
    void addProfile(const QString &);
    void addProfileFromSSURI(const QString &, QString);
    void save();
    void setJSONFile(const QString &);

private:
    bool autoHide;
    bool autoStart;
    bool debugLog;
    bool relativePath;
    bool translucent;
    bool useSystray;
    int m_index;
    QList<SSProfile> profileList;
    QString m_file;
    static bool tfo_available;
};

#endif // CONFIGURATION_H
