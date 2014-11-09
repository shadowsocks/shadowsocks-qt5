#include <QDir>
#include <QFile>
#include <QStringList>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDebug>
#include "ssvalidator.h"
#include "ssprofile.h"

SSProfile::SSProfile() :
    backend(),
    custom_arg(),
    fast_open(false),
    local_addr("127.0.0.1"),
    local_port("1080"),
    method("aes-256-cfb"),
    password(),
    profileName(),
    server(),
    server_port("8388"),
    timeout("600"),
    type("Shadowsocks-libev")
{ }

QByteArray SSProfile::getSsUrl()
{
    /*
     * cook a ss:// url
     */
    QString ssurl = QString("%1:%2@%3:%4").arg(method.toLower()).arg(password).arg(server).arg(server_port);
    QByteArray ba = QByteArray(ssurl.toStdString().c_str()).toBase64();
    ba.prepend("ss://");
    return ba;
}

void SSProfile::setBackend(bool relativePath)
{
    QString execName, sslocal;
    switch (this->getBackendType()) {
    case LIBEV:
        execName = "ss-local";
        break;
    case NODEJS:
        execName = "sslocal";
        break;
    case GO:
        execName = "shadowsocks-local";
        break;
    case PYTHON:
#ifdef Q_OS_WIN
        execName = "python";//detect python to avoid the conflict with sslocal.cmd of nodejs
#else
        execName = "sslocal";
#endif
        break;
    case LIBSHADOWSOCKS:
        backend.clear();
        return;
    default://shouldn't get here
        qWarning() << "Invalid backend type ID.";
        execName = "ss-local";
    }

#ifdef Q_OS_WIN
    QStringList findPathsList(QCoreApplication::applicationDirPath());
#else
    QStringList findPathsList(QDir::homePath() + "/.config/shadowsocks/bin");
#endif
    sslocal = QStandardPaths::findExecutable(execName, findPathsList);//search ss-qt5 directory first
    if(sslocal.isEmpty()) {//if not found then search system's PATH
        sslocal = QStandardPaths::findExecutable(execName);
    }
    this->setBackend(sslocal, relativePath);
}

void SSProfile::setBackend(const QString &a, bool relativePath)
{
    backend = QDir::toNativeSeparators(a);
#ifdef Q_OS_WIN
    if (type.compare("Shadowsocks-Python", Qt::CaseInsensitive) == 0) {
        QDir python(a);
        python.cdUp();
        QString scriptPath(python.absolutePath() + QString("/Scripts/sslocal-script.py"));
        if (QFile::exists(scriptPath)) {
            backend = QDir::toNativeSeparators(scriptPath);
        }
        else {
            backend = QString();
        }
    }
#endif
    if (relativePath) {
        QDir workingDir;
        backend = workingDir.relativeFilePath(backend);
    }
}

QString SSProfile::getBackend(bool relativePath)
{
    if (!isBackendMatchType()) {
        setBackend(relativePath);
    }
    return backend;
}

SSProfile::BackendType SSProfile::getBackendType() const
{
    if (type.compare("Shadowsocks-libev", Qt::CaseInsensitive) == 0) {
        return SSProfile::LIBEV;
    }
    if (type.compare("Shadowsocks-NodeJS", Qt::CaseInsensitive) == 0) {
        return SSProfile::NODEJS;
    }
    else if (type.compare("Shadowsocks-Go", Qt::CaseInsensitive) == 0) {
        return SSProfile::GO;
    }
    else if (type.compare("Shadowsocks-Python", Qt::CaseInsensitive) == 0) {
        return SSProfile::PYTHON;
    }
    else if (type.compare("Shadowsocks-libshadowsocks", Qt::CaseInsensitive) == 0) {
        return SSProfile::LIBSHADOWSOCKS;
    }
    else {
        return SSProfile::UNKNOWN;
    }
}

profile_t SSProfile::getLibshadowsocksProfile() const
{
    const profile_t profile = {
        .remote_host = server.toLatin1().data(),
        .local_addr = local_addr.toLatin1().data(),
        .method = method.toLatin1().data(),
        .password = password.toLatin1().data(),
        .remote_port = server_port.toInt(),
        .local_port = local_port.toInt(),
        .timeout = timeout.toInt(),

        .acl = NULL,
        .log = QDir::tempPath().append("/libshadowsocks.log").toLatin1().data(),
#ifdef __linux__
        .fast_open = fast_open ? 1 : 0,
#else
        .fast_open = 0,
#endif
        .udp_relay = 1,
        .verbose = 0
    };
    return profile;
}

bool SSProfile::isBackendMatchType()
{
    QFile file(backend);
    if (!file.exists()) {
        if (getBackendType() == SSProfile::LIBSHADOWSOCKS && backend.isEmpty()) {
            return true;
        }
        qWarning() << "Backend does not exist. You can safely ignore this message if you're changing the backend type.";
        return false;
    }

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!file.isReadable() || !file.isOpen()) {
        qWarning() << "Cannot read from backend.";
        return false;
    }

    SSProfile::BackendType rType;
    QString ident(file.readLine());
    file.close();
    if (ident.contains("node")) {
        rType = NODEJS;
    }
    else if (ident.contains("python")) {
        rType = PYTHON;
    }
    else {
        if (backend.contains("ss-local")) {
            rType = LIBEV;//libev
        }
        else {
            rType = GO;//Go
        }
    }

    return (rType == this->getBackendType());
}

bool SSProfile::isValid() const
{
    QFile backendFile(backend);
    bool valid = SSValidator::validatePort(server_port) && SSValidator::validatePort(local_port) && SSValidator::validateMethod(method);
    valid = valid && (backendFile.exists() || getBackendType() == SSProfile::LIBSHADOWSOCKS);

    if (server.isEmpty() || local_addr.isEmpty() || timeout.toInt() < 1 || !valid) {
        return false;
    }
    else {
        return true;
    }
}
