#include "mainwindow.h"
#include "connection.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QSharedMemory>
#include <QDebug>
#include <signal.h>

MainWindow *mw = nullptr;

static void onSignalRecv(int sig)
{
    if (sig == SIGUSR1) {
        if (mw) {
            mw->onShowSignalRecv();
        }
    }
    if (sig == SIGINT || sig == SIGTERM) qApp->quit();
}

int main(int argc, char *argv[])
{
    qRegisterMetaTypeStreamOperators<SQProfile>("SQProfile");

    QApplication a(argc, argv);

    signal(SIGINT, onSignalRecv);
    signal(SIGTERM, onSignalRecv);
    signal(SIGUSR1, onSignalRecv);

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
    mw = &w;

    QSharedMemory sharedMem;
    sharedMem.setKey("Shadowsocks-Qt5");
    if (w.isOnlyOneInstance()) {
        qint64 pid = -1;
        if (sharedMem.create(sizeof(qint64))) {
            pid = a.applicationPid();
            if (sharedMem.lock()) {
                *reinterpret_cast<qint64*>(sharedMem.data()) = pid;
                sharedMem.unlock();
            } else {
                qCritical() << sharedMem.errorString();
            }
        } else {
            if (!sharedMem.attach(QSharedMemory::ReadOnly)) {
                qCritical() << sharedMem.errorString();
            }
            if (sharedMem.lock()) {
                pid = *reinterpret_cast<qint64*>(sharedMem.data());
                sharedMem.unlock();
            } else {
                qCritical() << sharedMem.errorString();
            }

            //try to send a signal to show previous process's main window
            if (kill(pid, SIGUSR1) != 0) {
                QString errStr = QObject::tr("Failed to communicate with previously running instance of Shadowsocks-Qt5 (PID: %1). It might already crashed.").arg(pid);
                QMessageBox::critical(&w, QObject::tr("Error"), errStr);
            }
            //either way, this process has to quit
            return -1;
        }
    }

    w.show();
    if (w.isHideWindowOnStartup()) {
        if (w.isUsingAppIndicator()) {
            QTimer::singleShot(5, &w, SLOT(minimizeToSysTray()));
        } else {
            w.minimizeToSysTray();
        }
    }

    return a.exec();
}
