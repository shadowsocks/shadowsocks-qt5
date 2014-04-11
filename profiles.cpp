#include <QtDebug>
#include <QDir>
#include "profiles.h"

Profiles::Profiles(QString file)
{
    if (!file.isEmpty()) {
        setJSONFile(file);
    }
}

Profiles::~Profiles()
{}

void Profiles::setJSONFile(QString file)
{
    m_file = QDir::toNativeSeparators(file);
    QFile JSONFile(m_file);
    JSONFile.open(QIODevice::ReadOnly | QIODevice::Text);

    if (!JSONFile.isOpen()) {
        qWarning("Cannot open gui-config.json!");
    }

    QJsonParseError pe;
    JSONDoc = QJsonDocument::fromJson(JSONFile.readAll(), &pe);

    if (pe.error != QJsonParseError::NoError) {
        qWarning() << pe.errorString();
    }

    if (JSONDoc.isEmpty()) {
        qDebug() << m_file;
        qWarning("JSON Document is empty!");
    }

    JSONObj = JSONDoc.object();
    CONFArray = JSONObj["configs"].toArray();
    profileList.clear();//clear list before

    for (int i = 0; i< CONFArray.count(); i++) {
        QJsonObject json = CONFArray.at(i).toObject();
        SSProfile p;
        p.profileName = json["profile"].toString();
        p.server = json["server"].toString();
        p.password = json["password"].toString();
        p.server_port = json["server_port"].toString();
        p.local_port = json["local_port"].toString();
        p.method = json["method"].toString().toUpper();//using Upper-case in GUI
        p.timeout = json["timeout"].toString();
        profileList.append(p);
    }
    m_index = JSONObj["index"].toInt();
    app = JSONObj["app"].toString();
    JSONFile.close();
}

int Profiles::count()
{
    return profileList.count();
}

QStringList Profiles::getProfileList()
{
    QStringList s;
    for (int i = 0; i < profileList.count(); i++) {
        s.append(profileList.at(i).profileName);
    }
    return s;
}

SSProfile Profiles::getProfile(int index)
{
    return profileList.at(index);
}

SSProfile Profiles::lastProfile()
{
    return profileList.last();
}

void Profiles::addProfile(QString p)
{
    SSProfile n;
    n.profileName = p;
    profileList.append(n);

    QJsonObject json;
    json["profile"] = QJsonValue(n.profileName);
    CONFArray.append(QJsonValue(json));
}

void Profiles::saveProfile(int index, SSProfile p)
{
    profileList.replace(index, p);

    QJsonObject json;
    json["profile"] = QJsonValue(p.profileName);
    json["server"] = QJsonValue(p.server);
    json["server_port"] = QJsonValue(p.server_port);
    json["password"] = QJsonValue(p.password);
    json["local_port"] = QJsonValue(p.local_port);
    json["method"] = QJsonValue(p.method.toLower());//lower-case in config
    json["timeout"] = QJsonValue(p.timeout);
    CONFArray.replace(index, QJsonValue(json));
}

void Profiles::deleteProfile(int index)
{
    profileList.removeAt(index);
    CONFArray.removeAt(index);
}

void Profiles::saveProfileToJSON()
{
    JSONObj["index"] = QJsonValue(m_index);
    JSONObj["app"] = QJsonValue(app);
    JSONObj["configs"] = QJsonValue(CONFArray);

    JSONDoc.setObject(JSONObj);

    QFile JSONFile(m_file);
    JSONFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (JSONFile.isWritable()) {
        JSONFile.write(JSONDoc.toJson());
    }
    else {
        qWarning() << "Warning: file is not writable!";
    }
    JSONFile.close();
}

void Profiles::setIndex(int index)
{
    m_index = index;
}

int Profiles::getIndex()
{
    return m_index;
}

void Profiles::revert()
{
    setJSONFile(m_file);
}
