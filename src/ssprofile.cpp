#include <QDir>
#include <QFile>
#include <QStringList>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDebug>
#include "ssvalidator.h"
#include "ssprofile.h"

SSProfile::SSProfile() :
    backend(), type("Shadowsocks-libev"),  profileName(), server(), server_port("8388"), password(),
    local_addr("127.0.0.1"), local_port("1080"),  method("aes-256-cfb"), timeout("600"), fast_open(false)
{ }

void SSProfile::setBackend(bool relativePath)
{
    QString execName, sslocal;
    switch (this->getBackendTypeID()) {
    case 1://nodejs
        execName = "sslocal";
        break;
    case 2://go
        execName = "shadowsocks-local";
        break;
    case 3:
#ifdef _WIN32
        execName = "python";//detect python to avoid the conflict with sslocal.cmd of nodejs
#else
        execName = "sslocal";
#endif
        break;
    default://including 0. libev
        execName = "ss-local";
    }

#ifdef _WIN32
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
#ifdef _WIN32
    if (type.compare("Shadowsocks-Python", Qt::CaseInsensitive) == 0) {
        QDir python(a);
        python.cdUp();
        QString s(python.absolutePath() + QString("/Scripts/sslocal-script.py"));
        if (QFile::exists(s)) {
            backend = QDir::toNativeSeparators(s);
        }
        //return;
    }
#endif
    if (relativePath) {
        QDir workingDir;
        backend = workingDir.relativeFilePath(backend);
    }
}

QString SSProfile::getBackend()
{
    if (!isBackendMatchType()) {
        setBackend();
    }
    return backend;
}

int SSProfile::getBackendTypeID()
{
    if (type.compare("Shadowsocks-libev", Qt::CaseInsensitive) == 0) {
        return 0;
    }
    if (type.compare("Shadowsocks-NodeJS", Qt::CaseInsensitive) == 0) {
        return 1;
    }
    else if (type.compare("Shadowsocks-Go", Qt::CaseInsensitive) == 0) {
        return 2;
    }
    else if (type.compare("Shadowsocks-Python", Qt::CaseInsensitive) == 0) {
        return 3;
    }
    else {
        qWarning() << "Error. Unknown backend type.";
        return -1;
    }
}

bool SSProfile::isBackendMatchType()
{
    QFile file(backend);
    if (!file.exists()) {
        qWarning() << "Backend does not exist.";
        return false;
    }

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!file.isReadable() || !file.isOpen()) {
        qWarning() << "Cannot read from backend.";
        return false;
    }

    int rType = 0;
    QString ident(file.readLine());
    if (ident.contains("node")) {
        rType = 1;
    }
    else if (ident.contains("python")) {
        rType = 3;
    }
    else {
        if (backend.contains("ss-local")) {
            rType = 0;//libev
        }
        else
            rType = 2;//Go
    }

    return (rType == this->getBackendTypeID());
}

bool SSProfile::isValid() const
{
    bool valid;
    QFile backendFile(backend);
    valid = SSValidator::validatePort(server_port) && SSValidator::validatePort(local_port) && SSValidator::validateMethod(method) && backendFile.exists();

    //TODO: more accurate
    if (server.isEmpty() || local_addr.isEmpty() || timeout.toInt() < 1 || !valid) {
        return false;
    }
    else
        return true;
}
