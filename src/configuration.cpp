#include <QFile>
#include <QDir>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include "configuration.h"

Configuration::Configuration(const QString &file)
{
    setJSONFile(file);
}

Configuration::~Configuration()
{}

void Configuration::setJSONFile(const QString &file)
{
    m_file = QDir::toNativeSeparators(file);
    QFile JSONFile(m_file);

    if (!JSONFile.exists()) {
        qWarning() << "Warning: gui-config.json does not exist!";
        m_index = -1;
        debugLog = false;
        autoStart = false;
        autoHide = false;
        return;
    }

    JSONFile.open(QIODevice::ReadOnly | QIODevice::Text);

    if (!JSONFile.isOpen()) {
        qWarning() << "Critical Error: cannot open gui-config.json!";
    }

    if(!JSONFile.isReadable()) {
        qWarning() << "Critical Error: cannot read gui-config.json!";
    }

    QJsonParseError pe;
    QJsonDocument JSONDoc = QJsonDocument::fromJson(JSONFile.readAll(), &pe);

    if (pe.error != QJsonParseError::NoError) {
        qWarning() << pe.errorString();
    }

    if (JSONDoc.isEmpty()) {
        qWarning() << "Warning: JSON Document" << m_file << "is empty!";
    }

    QJsonObject JSONObj = JSONDoc.object();
    QJsonArray CONFArray = JSONObj["configs"].toArray();
    profileList.clear();//clear list before
    if (CONFArray.isEmpty()) {
        qWarning() << "configs is empty. Please check your gui-config.json";
        m_index = -1;//apparently m_index is invalid.
    }
    else {
        for (QJsonArray::iterator it = CONFArray.begin(); it != CONFArray.end(); ++it) {
            QJsonObject json = (*it).toObject();
            SSProfile p;
            p.backend = json["backend"].toString();
            p.type = json["type"].toString();
            p.profileName = json["profile"].toString();
            p.server = json["server"].toString();
            p.password = json["password"].toString();
            p.server_port = json["server_port"].toString();
            p.local_addr = json["local_address"].toString();
            p.local_port = json["local_port"].toString();
            p.method = json["method"].toString().toUpper();//using Upper-case in GUI
            p.timeout = json["timeout"].toString();
            profileList << p;
        }
        m_index = JSONObj["index"].toInt();
    }
    debugLog = JSONObj["debug"].toBool();
    autoHide = JSONObj["autoHide"].toBool();
    autoStart = JSONObj["autoStart"].toBool();
    translucent = JSONObj["translucent"].toBool();
    JSONFile.close();
}

int Configuration::count()
{
    return profileList.count();
}

QStringList Configuration::getProfileList()
{
    QStringList s;
    for (QList<SSProfile>::iterator it = profileList.begin(); it != profileList.end(); ++it) {
        s << it->profileName;
    }
    return s;
}

void Configuration::addProfile(const QString &pName)
{
    SSProfile p;
    p.profileName = pName;
    profileList << p;

    QJsonObject json;
    json["profile"] = QJsonValue(p.profileName);
    //below are using default values
    json["type"] = QJsonValue(p.type);
    json["server_port"] = QJsonValue(p.server_port);
    json["local_address"] = QJsonValue(p.local_addr);
    json["local_port"] = QJsonValue(p.local_port);
    json["method"] = QJsonValue(p.method);
    json["timeout"] = QJsonValue(p.timeout);
}

void Configuration::addProfileFromSSURI(const QString &name, QString uri)
{
    SSProfile p;
    p.profileName = name;

    QStringList resultList = QString(QByteArray::fromBase64(uri.toLatin1())).split(':');
    p.method = resultList.first();
    p.server_port = resultList.last();
    QStringList ser = resultList.at(1).split('@');
    p.server = ser.last();
    ser.removeLast();
    p.password = ser.join('@');//incase there is a '@' in password

    QJsonObject json;
    json["profile"] = QJsonValue(p.profileName);
    json["server"] = QJsonValue(p.server);
    json["password"] = QJsonValue(p.password);
    json["method"] = QJsonValue(p.method.toLower());
    json["server_port"] = QJsonValue(p.server_port);
    //below are using default values
    json["type"] = QJsonValue(p.type);
    json["local_address"] = QJsonValue(p.local_addr);
    json["local_port"] = QJsonValue(p.local_port);
    json["timeout"] = QJsonValue(p.timeout);
    profileList << p;
}

void Configuration::deleteProfile(int index)
{
    profileList.removeAt(index);
}

void Configuration::save()
{
    QJsonArray newConfArray;
    for (QList<SSProfile>::iterator it = profileList.begin(); it != profileList.end(); ++it) {
        QJsonObject json;
        json["backend"] = QJsonValue(it->backend);
        json["type"] = QJsonValue(it->type);
        json["profile"] = QJsonValue(it->profileName);
        json["server"] = QJsonValue(it->server);
        json["server_port"] = QJsonValue(it->server_port);
        json["password"] = QJsonValue(it->password);
        json["local_address"] = QJsonValue(it->local_addr);
        json["local_port"] = QJsonValue(it->local_port);
        json["method"] = QJsonValue(it->method.isEmpty() ? QString("table") : it->method.toLower());//lower-case in config
        json["timeout"] = QJsonValue(it->timeout);
        newConfArray << QJsonValue(json);
    }

    QJsonObject JSONObj;
    JSONObj["index"] = QJsonValue(m_index);
    JSONObj["debug"] = QJsonValue(debugLog);
    JSONObj["autoHide"] = QJsonValue(autoHide);
    JSONObj["autoStart"] = QJsonValue(autoStart);
    JSONObj["translucent"] = QJsonValue(translucent);
    JSONObj["configs"] = QJsonValue(newConfArray);

    QJsonDocument JSONDoc(JSONObj);

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

void Configuration::setIndex(int index)
{
    m_index = index;
}

int Configuration::getIndex()
{
    return m_index;
}

bool Configuration::isDebug()
{
    return debugLog;
}

void Configuration::setDebug(bool d)
{
    debugLog = d;
}

bool Configuration::isAutoStart()
{
    return autoStart;
}

void Configuration::setAutoStart(bool s)
{
    autoStart = s;
}

void Configuration::setAutoHide(bool h)
{
    autoHide = h;
}

bool Configuration::isAutoHide()
{
    return autoHide;
}

void Configuration::setTranslucent(bool t)
{
    translucent = t;
}

bool Configuration::isTranslucent()
{
    return translucent;
}

void Configuration::revert()
{
    setJSONFile(m_file);
}
