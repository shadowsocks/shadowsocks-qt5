#include <QFile>
#include <QDir>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include "configuration.h"

#ifdef Q_OS_LINUX
#include <sys/utsname.h>
#endif

bool Configuration::tfo_available = false;

Configuration::Configuration(const QString &file)
{
#ifdef Q_OS_LINUX
    /*
     * Determine linux kernel version on the run-time
     * if it's greater than 3.7, then define tcp fast open available.
     */
    utsname u;
    uname(&u);
    QString linux_release(u.release);
    tfo_available = (linux_release.mid(2, 2).toFloat() >= 7 && linux_release.at(0) == '3') ? true : false;
#endif
    setJSONFile(file);
}

Configuration::~Configuration()
{}

void Configuration::setJSONFile(const QString &file)
{
    profileList.clear();//clear list in the very beginning
    m_file = QDir::toNativeSeparators(file);
    QFile JSONFile(m_file);

    if (!JSONFile.exists()) {
        qWarning() << "Warning: gui-config.json does not exist!";
        autoHide = false;
        autoStart = false;
        debugLog = false;
        m_index = -1;
        relativePath = false;
        translucent = true;
        return;
    }

    JSONFile.open(QIODevice::ReadOnly | QIODevice::Text);

    if (!JSONFile.isOpen()) {
        qCritical() << "Critical Error: cannot open gui-config.json!";
    }

    if(!JSONFile.isReadable()) {
        qCritical() << "Critical Error: cannot read gui-config.json!";
    }

    QJsonParseError pe;
    QJsonDocument JSONDoc = QJsonDocument::fromJson(JSONFile.readAll(), &pe);

    if (pe.error != QJsonParseError::NoError) {
        qCritical() << pe.errorString();
    }

    if (JSONDoc.isEmpty()) {
        qWarning() << "Warning: JSON Document" << m_file << "is empty!";
    }

    QJsonObject JSONObj = JSONDoc.object();
    QJsonArray CONFArray = JSONObj["configs"].toArray();
    if (CONFArray.isEmpty()) {
        qWarning() << "configs is empty. Please check your gui-config.json";
        m_index = -1;//apparently m_index is invalid.
    }
    else {
        for (QJsonArray::iterator it = CONFArray.begin(); it != CONFArray.end(); ++it) {
            QJsonObject json = (*it).toObject();
            SSProfile p;
            p.backend = json["backend"].toString();
            p.custom_arg = json["custom_arg"].toString();
            p.local_addr = json["local_address"].toString();
            p.local_port = json["local_port"].toString();
            p.method = json["method"].toString().toUpper();//using Upper-case in GUI
            p.password = json["password"].toString();
            p.profileName = json["profile"].toString();
            p.server = json["server"].toString();
            p.server_port = json["server_port"].toString();
            p.timeout = json["timeout"].toString();
            p.type = json["type"].toString();
            if (tfo_available) {
                p.fast_open = json["fast_open"].toBool();
            }
            profileList << p;
        }
        m_index = JSONObj["index"].toInt();
    }
    autoHide = JSONObj["autoHide"].toBool();
    autoStart = JSONObj["autoStart"].toBool();
    debugLog = JSONObj["debug"].toBool();
    relativePath = JSONObj["relative_path"].toBool();
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
}

void Configuration::addProfileFromSSURI(const QString &name, QString uri)
{
    SSProfile p;
    p.profileName = name;

    uri.remove(0, 5);//remove the prefix "ss://" from uri
    QStringList resultList = QString(QByteArray::fromBase64(QByteArray(uri.toStdString().c_str()))).split(':');
    p.method = resultList.first().toUpper();
    p.server_port = resultList.last();
    QStringList ser = resultList.at(1).split('@');
    p.server = ser.last();
    ser.removeLast();
    p.password = ser.join('@');//incase there is a '@' in password

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
        json["custom_arg"] = QJsonValue(it->custom_arg);
        json["local_address"] = QJsonValue(it->local_addr);
        json["local_port"] = QJsonValue(it->local_port);
        json["method"] = QJsonValue(it->method.isEmpty() ? QString("table") : it->method.toLower());//lower-case in config
        json["password"] = QJsonValue(it->password);
        json["profile"] = QJsonValue(it->profileName);
        json["server_port"] = QJsonValue(it->server_port);
        json["server"] = QJsonValue(it->server);
        json["timeout"] = QJsonValue(it->timeout);
        json["type"] = QJsonValue(it->type);
        if (tfo_available) {
            json["fast_open"] = QJsonValue(it->fast_open);
        }
        newConfArray.append(QJsonValue(json));
    }

    QJsonObject JSONObj;
    JSONObj["autoHide"] = QJsonValue(autoHide);
    JSONObj["autoStart"] = QJsonValue(autoStart);
    JSONObj["configs"] = QJsonValue(newConfArray);
    JSONObj["debug"] = QJsonValue(debugLog);
    JSONObj["index"] = QJsonValue(m_index);
    JSONObj["relative_path"] = QJsonValue(relativePath);
    JSONObj["translucent"] = QJsonValue(translucent);

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

void Configuration::setRelativePath(bool r)
{
    relativePath = r;
}

bool Configuration::isRelativePath()
{
    return relativePath;
}

void Configuration::revert()
{
    setJSONFile(m_file);
}
