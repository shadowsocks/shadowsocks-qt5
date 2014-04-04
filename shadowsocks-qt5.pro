#-------------------------------------------------
#
#          Project Shadowsocks-Qt5
#
#-------------------------------------------------

QT       += core gui widgets

TARGET   = ss-qt5
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    profiles.cpp \
    ss_process.cpp \
    logdialogue.cpp

HEADERS  += mainwindow.h \
    profiles.h \
    ss_process.h \
    logdialogue.h

FORMS    += mainwindow.ui \
    logdialogue.ui

OTHER_FILES += \
    README.md \
    gui-config.json

RESOURCES += icons.qrc

RC_FILE = ss-qt5.rc
