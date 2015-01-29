#include "mainwindow.h"
#include "ss_process.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QSharedMemory>

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

    QTranslator ssqt5t;
    ssqt5t.load(QLocale::system(), "ss-qt5", "_", ":/i18n");
    a.installTranslator(&ssqt5t);

    MainWindow w(a.arguments().contains("-v"));

    QSharedMemory sharedMem;
    sharedMem.setKey("shadowsocks-qt5");
    if (!sharedMem.create(1) && w.m_conf->isSingleInstance()) {
        QMessageBox::critical(&w, QObject::tr("Error"), QObject::tr("Another instance of Shadowsocks-Qt5 is already running."));
        return -1;
    }

    w.show();
    if (w.m_conf->isAutoStart()) {
        w.onStartButtonPressed();
    }
    if (w.m_conf->isAutoHide()) {
        w.minimizeToSysTray();
    }

    return a.exec();
}
