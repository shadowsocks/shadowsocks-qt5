#-------------------------------------------------
#
#          Project Shadowsocks-Qt5
#
#-------------------------------------------------

QT        += core gui widgets network
unix: QT  += dbus

CONFIG    += c++11

TARGET     = ss-qt5
TEMPLATE   = app
VERSION    = 2.6.0
DEFINES   += APP_VERSION=\\\"$$VERSION\\\"

include(src/ss-qt5.pri)

OTHER_FILES  += README.md \
                shadowsocks-qt5.desktop

desktop.files = shadowsocks-qt5.desktop
ssicon.files  = src/icons/shadowsocks-qt5.png

isEmpty(INSTALL_PREFIX) {
    unix: INSTALL_PREFIX = /usr
    else: INSTALL_PREFIX = ..
}

unix: {
    desktop.path  = $$INSTALL_PREFIX/share/applications
    ssicon.path   = $$INSTALL_PREFIX/share/icons/hicolor/512x512/apps
    INSTALLS     += desktop ssicon
}
win32: DEFINES   += QSS_STATIC

target.path       = $$INSTALL_PREFIX/bin

INSTALLS         += target
