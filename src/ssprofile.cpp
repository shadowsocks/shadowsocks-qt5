#include <QDir>
#include <QFile>
#include <QStringList>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDebug>
#include "ssprofile.h"

SSProfile::SSProfile() :
    backend(), type("Shadowsocks-libev"),  profileName(), server(), server_port("8388"), password(),
    local_addr("127.0.0.1"), local_port("1080"),  method("aes-256-cfb"), timeout("600")
{ }

void SSProfile::setBackend()
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

    if(!sslocal.isEmpty()) {
        this->setBackend(sslocal);
    }
}

void SSProfile::setBackend(const QString &a)
{
#ifdef _WIN32
    if (type == QString("Shadowsocks-Python")) {
        QDir python(a);
        python.cdUp();
        QString s(python.absolutePath() + QString("/Scripts/sslocal-script.py"));
        if (QFile::exists(s)) {
            backend = QDir::toNativeSeparators(s);
        }
        return;
    }
#endif
    backend = QDir::toNativeSeparators(a);
}

int SSProfile::getBackendTypeID()
{
    if (type == QString("Shadowsocks-Nodejs")) {
        return 1;
    }
    else if (type == QString("Shadowsocks-Go")) {
        return 2;
    }
    else if (type == QString("Shadowsocks-Python")) {
        return 3;
    }
    else {
        return 0;//Shadowsocks-libev
    }
}

bool SSProfile::isBackendMatchType()
{
    QFile file(backend);
    if (!file.exists()) {
        qWarning() << "Detecting backend does not exist.";
        return false;
    }

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!file.isReadable() || !file.isOpen()) {
        qWarning() << "Cannot read from detecting backend.";
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
