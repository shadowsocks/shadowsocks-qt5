#include <QtDebug>
#include "ss_process.h"

SS_Process::SS_Process(QObject *parent) :
    QObject(parent)
{
    proc.setReadChannelMode(QProcess::MergedChannels);
    connect(&proc, &QProcess::readyRead, this, &SS_Process::autoemitreadReadyProcess);
    connect(&proc, &QProcess::started, this, &SS_Process::started);
    connect(&proc, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, &SS_Process::exited);
}

SS_Process::~SS_Process()
{}

void SS_Process::setapp(const QString &a)
{
    app_path = a;
}

bool SS_Process::isRunning()
{
    return running;
}

void SS_Process::start(SSProfile &p, bool debug)
{
    start(p.server, p.password, p.server_port, p.local_port, p.method, p.timeout, debug);
}

void SS_Process::start(const QString &args)
{
    stop();
#ifdef _WIN32
    proc.setProgram(app_path);
    proc.setNativeArguments(args);
    proc.start();
#else
    //setNativeArguments is not available except Windows and Symbian
    proc.start(app_path + QString(" ") + args);
#endif
    qDebug() << "Backend arguments are " << args;
    proc.waitForStarted(1000);//wait for at most 1 second
}

void SS_Process::start(const QString &server, const QString &pwd, const QString &s_port, const QString &l_port, const QString &method, const QString &timeout, bool debug)
{
    QString args;
    args.append(QString("-s ") + server);
    args.append(QString(" -p ") + s_port);
    args.append(QString(" -l ") + l_port);
    args.append(QString(" -k \"") + pwd + QString("\""));
    args.append(QString(" -m ") + method.toLower());
    args.append(QString(" -t ") + timeout);//shadowsocks-go doesn't support this argument
    if (debug) {
        args.append(QString(" -v"));//shadowsocks-go uses "-d=true"
    }

    start(args);
}

void SS_Process::stop()
{
    if (proc.isOpen()) {
        proc.close();
    }
}

void SS_Process::autoemitreadReadyProcess()
{
    emit readReadyProcess(proc.readAll());
}

void SS_Process::started()
{
    running = true;
    qDebug() << "Backend started";
    emit sigstart();
}

void SS_Process::exited(int e)
{
    qDebug() << "Backend exited. Exit Code: " << e;
    running = false;
    emit sigstop();
}
