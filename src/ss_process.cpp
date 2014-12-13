#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include "ss_process.h"

SS_Process::SS_Process(QObject *parent) :
    QObject(parent)
{
    libQSS = false;
    qssController = NULL;
    proc.setProcessChannelMode(QProcess::MergedChannels);

    connect(&proc, &QProcess::readyRead, this, &SS_Process::onProcessReadyRead);
    connect(&proc, &QProcess::started, this, &SS_Process::onStarted);
    connect(&proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &SS_Process::onExited);
}

SS_Process::~SS_Process()
{}

void SS_Process::start(SSProfile * const p, bool debug)
{
    app_path = p->backend;
    backendType = p->getBackendType();
    stop();
    if (backendType == SSProfile::LIBQSS) {
        libQSS = true;
        startQSS(p, debug);
    }
    else {
        libQSS = false;
        start(p->server, p->password, p->server_port, p->local_addr, p->local_port, p->method, p->timeout, p->custom_arg, debug, p->fast_open);
    }
}

void SS_Process::start(QString &args)
{
#ifdef Q_OS_WIN
    QString sslocalbin = QFileInfo(app_path).dir().canonicalPath();
    sslocalbin.append("/node_modules/shadowsocks/bin/sslocal");
    sslocalbin = QString("\"") + QDir::toNativeSeparators(sslocalbin) + QString("\"");
    switch (backendType) {
    case SSProfile::LIBEV:
        args.append(" -u");
    case SSProfile::GO:
        proc.setProgram(app_path);
        break;
    case SSProfile::NODEJS:
        proc.setProgram("node");
        args.prepend(sslocalbin);
        break;
    case SSProfile::PYTHON:
        proc.setProgram("python");
        args.prepend(QString("\""));
        args.prepend(app_path);
        args.prepend(QString("\""));
        break;
    default:
        qWarning() << tr("Aborted: Invalid Backend Type.") << backendType;
        return;
    }
    proc.setNativeArguments(args);
    proc.start();
#else
    proc.start(app_path + QString(" ") + args);
#endif
    qDebug() << tr("Backend arguments are ") << args;
    proc.waitForStarted(1000);//wait for at most 1 second
}

void SS_Process::startQSS(SSProfile * const p, bool debug)
{
    QSS::Profile profile = p->getQSSProfile();
    if (qssController == NULL) {
        qssController = new QSS::Controller(profile, true, this);
        if (debug) {
            connect(qssController, &QSS::Controller::info, this, &SS_Process::onQSSInfoReady, Qt::DirectConnection);
        }
        connect(qssController, &QSS::Controller::error, this, &SS_Process::onQSSInfoReady, Qt::DirectConnection);
    }
    if (qssController->start()) {
        emit processStarted();
    }
    else {
        qssController->deleteLater();
        qssController = NULL;
    }
}

void SS_Process::start(const QString &server, const QString &pwd, const QString &s_port, const QString &l_addr, const QString &l_port, const QString &method, const QString &timeout, const QString &custom_arg, bool debug, bool tfo)
{
    QString args;
    args.append(QString(" -s ") + server);
    args.append(QString(" -p ") + s_port);
    args.append(QString(" -b ") + l_addr);
    args.append(QString(" -l ") + l_port);
    args.append(QString(" -k \"") + pwd + QString("\""));
    args.append(QString(" -m ") + method.toLower());
    if (backendType != SSProfile::GO) {//go port doesn't support this argument
        args.append(QString(" -t ") + timeout);
    }

    if (debug) {
        if(backendType == SSProfile::GO) {
            args.append("-d=true");
        }
        else {
            args.append(" -v");
        }
    }

#ifdef Q_OS_LINUX
    if ((backendType == SSProfile::PYTHON || backendType == SSProfile::LIBEV) && tfo) {//only python and libev ports support tfo
        args.append(" --fast-open");
    }
#else
    Q_UNUSED(tfo);
#endif

    args.append(" ").append(custom_arg);
    start(args);
}

void SS_Process::stop()
{
    if (libQSS && qssController != NULL) {
        qssController->stop();
        qssController->deleteLater();
        qssController = NULL;
        emit processStopped();
    }
    else if (proc.isOpen()) {
        proc.close();
    }
}

void SS_Process::onProcessReadyRead()
{
    emit processRead(proc.readAll());
}

void SS_Process::onQSSInfoReady(const QString &s)
{
    emit processRead(s.toLocal8Bit());
}

void SS_Process::onStarted()
{
    qDebug() << tr("Backend started. PID: ") <<proc.pid();
    emit processStarted();
}

void SS_Process::onExited(int e)
{
    qDebug() << tr("Backend exited. Exit Code: ") << e;
    emit processStopped();
}
