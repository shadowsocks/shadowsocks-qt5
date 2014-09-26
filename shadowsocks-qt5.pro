#-------------------------------------------------
#
#          Project Shadowsocks-Qt5
#
#-------------------------------------------------

QT      += core gui widgets
win32: QT += winextras

TARGET   = ss-qt5
TEMPLATE = app
VERSION  = 0.4.3
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

include(src/ss-qt5.pri)

OTHER_FILES  += README.md \
                gui-config.json \
                shadowsocks-qt5.desktop

desktop.files = shadowsocks-qt5.desktop
ssicon.files  = src/icon/shadowsocks-qt5.png

isEmpty(INSTALL_PREFIX) {
    unix: INSTALL_PREFIX = /usr
    else: INSTALL_PREFIX = ..
}
unix: {
    desktop.path  = $$INSTALL_PREFIX/share/applications
    ssicon.path   = $$INSTALL_PREFIX/share/icons/hicolor/512x512/apps
}

target.path = $$INSTALL_PREFIX/bin

INSTALLS += target

!mac:unix: INSTALLS += desktop ssicon
