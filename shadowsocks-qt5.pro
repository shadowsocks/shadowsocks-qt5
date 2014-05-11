#-------------------------------------------------
#
#          Project Shadowsocks-Qt5
#
#-------------------------------------------------

QT      += core gui widgets
win32: QT += winextras

TARGET   = ss-qt5
TEMPLATE = app
VERSION  = 0.4.1

include(src/ss-qt5.pri)

OTHER_FILES  += README.md \
                gui-config.json \
                shadowsocks-qt5.desktop

binary_path   = /usr/bin
desktop.path  = /usr/share/applications
desktop.files = shadowsocks-qt5.desktop
ssicon.path   = /usr/share/icons/hicolor/512x512/apps
ssicon.files  = icon/shadowsocks-qt5.png

!equals($$(INSTALL_PREFIX), "") {
    unix: {
        binary_path   = $$(INSTALL_PREFIX)/bin
        desktop.path  = $$(INSTALL_PREFIX)/share/applications
        ssicon.path   = $$(INSTALL_PREFIX)/share/icons/hicolor/512x512/apps
    }
    else: binary_path = $$(INSTALL_PREFIX)
}

target.path = $$binary_path

win32: {
    gui_conf.path  = target.path
    gui_conf.files = gui-config.json
    INSTALLS      += gui_conf
}

INSTALLS += target

!mac:unix: INSTALLS += desktop ssicon
