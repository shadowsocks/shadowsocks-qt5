#-------------------------------------------------
#
#          Project Shadowsocks-Qt5
#
#-------------------------------------------------

QT      += core gui widgets
win32: QT += winextras
linux: QT += dbus

TARGET   = ss-qt5
TEMPLATE = app
VERSION  = 0.5.0
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
    iconsymlink.path=.
    iconsymlink.commands=ln -s $$INSTALL_PREFIX/share/icons/hicolor/512x512/apps/shadowsocks-qt5.png $$INSTALL_PREFIX/share/pixmaps/shadowsocks-qt5.png
    INSTALLS += desktop ssicon iconsymlink
}

target.path = $$INSTALL_PREFIX/bin

INSTALLS += target
