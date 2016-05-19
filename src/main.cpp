#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QSharedMemory>
#include <QDebug>
#include <QDir>
#include <QCommandLineParser>
#include <signal.h>
#include "mainwindow.h"
#include "confighelper.h"

MainWindow *mainWindow = nullptr;

static void onSignalRecv(int sig)
{
#ifdef Q_OS_UNIX
    if (sig == SIGUSR1) {
        if (mainWindow) {
            mainWindow->show();
        }
    }
#endif
    if (sig == SIGINT || sig == SIGTERM) qApp->quit();
}

bool isConflictWithPreviousInstance(QApplication &a)
{
    QSharedMemory *sharedMem = new QSharedMemory("Shadowsocks-Qt5", &a);
    qint64 pid = -1;

    if (sharedMem->create(sizeof(qint64))) {
        pid = QCoreApplication::applicationPid();
        if (sharedMem->lock()) {
            *reinterpret_cast<qint64*>(sharedMem->data()) = pid;
            sharedMem->unlock();
            // we can only run a new instance of ss-qt5 if the shared memory
            // is created and written successfully
            return false;
        } else {
            qCritical() << sharedMem->errorString();
        }
    } else {
        if (!sharedMem->attach(QSharedMemory::ReadOnly)) {
            qCritical() << sharedMem->errorString();
        } else if (sharedMem->lock()) {
            pid = *reinterpret_cast<qint64*>(sharedMem->data());
            sharedMem->unlock();
        } else {
            qCritical() << sharedMem->errorString();
        }

#ifdef Q_OS_UNIX
        //try to send a signal to show previous process's main window
        if (kill(pid, SIGUSR1) != 0) {
            QString errStr = QObject::tr("Failed to communicate with previously running instance of Shadowsocks-Qt5 (PID: %1). It might already crashed.").arg(pid);
            QMessageBox::critical(mainWindow, QObject::tr("Error"), errStr);
        }
#else
        QMessageBox::critical(mainWindow, QObject::tr("Error"), QObject::tr("Another instance of Shadowsocks-Qt5 (PID: %1) is already running.").arg(pid));
#endif
    }
    // Can't run a new instance under all other situations
    return true;
}

void setupApplication(QApplication &a)
{
    signal(SIGINT, onSignalRecv);
    signal(SIGTERM, onSignalRecv);
#ifdef Q_OS_UNIX
    signal(SIGUSR1, onSignalRecv);
#endif

    a.setApplicationName(QString("shadowsocks-qt5"));
    a.setApplicationDisplayName(QString("Shadowsocks-Qt5"));
    a.setApplicationVersion(APP_VERSION);

#ifdef Q_OS_WIN
    if (QLocale::system().country() == QLocale::China) {
        a.setFont(QFont("Microsoft Yahei", 9, QFont::Normal, false));
    }
    else {
        a.setFont(QFont("Segoe UI", 9, QFont::Normal, false));
    }
#endif
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
    QIcon::setThemeName("Breeze");
#endif

    QTranslator *ssqt5t = new QTranslator(&a);
    ssqt5t->load(QLocale::system(), "ss-qt5", "_", ":/i18n");
    a.installTranslator(ssqt5t);
}

int main(int argc, char *argv[])
{
    qRegisterMetaTypeStreamOperators<SQProfile>("SQProfile");

    QApplication a(argc, argv);
    setupApplication(a);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption configFileOption("c",
                                        "specify configuration file.",
                                        "config.ini");
    parser.addOption(configFileOption);
    parser.process(a);

    QString configFile = parser.value(configFileOption);
    if (configFile.isEmpty()) {
#ifdef Q_OS_WIN
        configFile = a.applicationDirPath() + "/config.ini";
#else
        QDir configDir = QDir::homePath() + "/.config/shadowsocks-qt5";
        configFile = configDir.absolutePath() + "/config.ini";
        if (!configDir.exists()) {
            configDir.mkpath(configDir.absolutePath());
        }
#endif
    }
    ConfigHelper conf(configFile);

    if (conf.isOnlyOneInstance() && isConflictWithPreviousInstance(a)) {
        return -1;
    }

    MainWindow w(&conf);
    mainWindow = &w;

    w.startAutoStartConnections();

    if (!conf.isHideWindowOnStartup()) {
        w.show();
    }

    return a.exec();
}
