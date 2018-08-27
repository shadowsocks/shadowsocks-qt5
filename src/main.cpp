#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QCommandLineParser>
#include <signal.h>
#include "mainwindow.h"
#include "confighelper.h"

MainWindow *mainWindow = nullptr;

static void onSignalRecv(int sig)
{
    if (sig == SIGINT || sig == SIGTERM) {
        qApp->quit();
    } else {
        qWarning("Unhandled signal %d", sig);
    }
}

void setupApplication(QApplication &a)
{
    signal(SIGINT, onSignalRecv);
    signal(SIGTERM, onSignalRecv);

    QApplication::setFallbackSessionManagementEnabled(false);

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

    MainWindow w(&conf);
    mainWindow = &w;

    if (conf.isOnlyOneInstance() && w.isInstanceRunning()) {
        return -1;
    }

    w.startAutoStartConnections();

    if (!conf.isHideWindowOnStartup()) {
        w.show();
    }

    return a.exec();
}
