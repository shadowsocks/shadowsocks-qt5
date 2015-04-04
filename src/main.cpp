#include "mainwindow.h"
#include "connection.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QSharedMemory>
#include <signal.h>

static void onSIGINT_TERM(int sig)
{
    if (sig == SIGINT || sig == SIGTERM) qApp->quit();
}

int main(int argc, char *argv[])
{
    qRegisterMetaTypeStreamOperators<SQProfile>("SQProfile");

    QApplication a(argc, argv);

    signal(SIGINT, onSIGINT_TERM);
    signal(SIGTERM, onSIGINT_TERM);

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
    QIcon::setThemeName("Breeze");
#endif

    QTranslator ssqt5t;
    ssqt5t.load(QLocale::system(), "ss-qt5", "_", ":/i18n");
    a.installTranslator(&ssqt5t);

    MainWindow w;
/*
    QSharedMemory sharedMem;
    sharedMem.setKey("shadowsocks-qt5");
    if (!sharedMem.create(1) && w.m_conf->isSingleInstance()) {
        QMessageBox::critical(&w, QObject::tr("Error"), QObject::tr("Another instance of Shadowsocks-Qt5 is already running."));
        return -1;
    }
*/

    w.show();

    return a.exec();
}
