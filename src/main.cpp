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

#ifdef _WIN32
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
    ssqt5t.load("ssqt5_" + QLocale::system().name(), QCoreApplication::applicationDirPath());
    a.installTranslator(&ssqt5t);

    MainWindow w;
    w.show();

    if (w.m_conf->isAutoStart()) {
        w.startButtonPressed();
    }
    if (w.m_conf->isAutoHide()) {
        w.minimizeToSysTray();
    }

    return a.exec();
}
