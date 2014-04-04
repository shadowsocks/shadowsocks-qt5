#include <QtDebug>
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
    m_file = file;
    QFile JSONFile(m_file);
    JSONFile.open(QIODevice::ReadOnly | QIODevice::Text);
    JSONDoc = QJsonDocument::fromJson(JSONFile.readAll());

    if (JSONDoc.isEmpty()) {
        qWarning("JSON Document is empty!");
    }

    JSONObj = JSONDoc.object();
    CONFArray = JSONObj["configs"].toArray();

    for (int i = 0; i< CONFArray.count(); i++) {
        QJsonObject json = CONFArray.at(i).toObject();
        SSProfile p;
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

QStringList Profiles::getserverList()
{
    QStringList s;
    for (int i = 0; i < profileList.count(); i++) {
        s.append(profileList.at(i).server);
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

/*
QString Profiles::getLocal_port(int index)
{
    return profileList.at(index).local_port;
}

QString Profiles::getMethod(int index)
{
    return profileList.at(index).method;
}

QString Profiles::getPassword(int index)
{
    return profileList.at(index).password;
}

QString Profiles::getServer_port(int index)
{
    return profileList.at(index).server_port;
}

QString Profiles::getSever(int index)
{
    return profileList.at(index).server;
}

QString Profiles::getTimeout(int index)
{
    return profileList.at(index).timeout;
}
*/

void Profiles::addProfile(QString s)
{
    SSProfile n;
    n.server = s;
    profileList.append(n);

    QJsonObject json;
    json["server"] = QJsonValue(n.server);
    CONFArray.append(QJsonValue(json));
}

void Profiles::saveProfile(int index, SSProfile p)
{
    profileList.replace(index, p);

    QJsonObject json;
    json["server"] = QJsonValue(p.server);
    json["server_port"] = QJsonValue(p.server_port);
    json["password"] = QJsonValue(p.password);
    json["local_port"] = QJsonValue(p.local_port);
    json["method"] = QJsonValue(p.method.toLower());//lower-case in config
    json["timeout"] = QJsonValue(p.timeout);
    CONFArray.replace(index, QJsonValue(json));
}
/*
void Profiles::saveProfile(int index, QString s, QString sport, QString pwd, QString lport, QString m, QString t)
{
    SSProfile p;
    p.server = s;
    p.server_port = sport;
    p.password = pwd;
    p.local_port = lport;
    p.method = m;
    p.timeout = t;
    profileList.replace(index, p);

    QJsonObject json;
    json["server"] = QJsonValue(s);
    json["server_port"] = QJsonValue(sport);
    json["password"] = QJsonValue(pwd);
    json["local_port"] = QJsonValue(lport);
    json["method"] = QJsonValue(m.toLower());//lower-case in config
    json["timeout"] = QJsonValue(t);
    CONFArray.replace(index, QJsonValue(json));
}*/

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
}

void Profiles::setIndex(int index)
{
    m_index = index;
}

int Profiles::getIndex()
{
    return m_index;
}
