#include "mainwindow.h"
#include "profiles.h"
#include "ss_process.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    if (w.m_profile->isAutoStart()) {
        w.startButtonPressed();
    }
    if (w.m_profile->isAutoHide()) {
        w.hide();
    }

    return a.exec();
}
