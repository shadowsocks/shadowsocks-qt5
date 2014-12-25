#include "mainwindow.h"
#include "ss_process.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName(QString("shadowsocks-qt5"));
    a.setApplicationDisplayName(QString("Shadowsocks Qt5"));
    a.setApplicationVersion(APP_VERSION);

#ifdef Q_OS_WIN
    if (QLocale::system().country() == QLocale::China) {
        a.setFont(QFont("Microsoft Yahei", 9, QFont::Normal, false));
    }
    else {
        a.setFont(QFont("Segoe UI", 9, QFont::Normal, false));
    }
#endif

    QTranslator t;
    t.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&t);

    QTranslator ssqt5t;
    #ifdef Q_OS_WIN
    ssqt5t.load(QLocale::system().name(), "ss-qt5", "_", QCoreApplication::applicationDirPath());
    #else
    ssqt5t.load(QLocale::system().name(), "ss-qt5", "_", QString(LOC_PREFIX).append("/share/shadowsocks-qt5"));
    #endif
    a.installTranslator(&ssqt5t);

    MainWindow w(a.arguments().contains("-v"));
    w.show();

    if (w.m_conf->isAutoStart()) {
        w.onStartButtonPressed();
    }
    if (w.m_conf->isAutoHide()) {
        w.minimizeToSysTray();
    }

    return a.exec();
}
