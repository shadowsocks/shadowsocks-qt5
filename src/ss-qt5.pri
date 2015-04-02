SOURCES      += src/main.cpp\
                src/mainwindow.cpp \
                src/ss_process.cpp \
                src/ip4validator.cpp \
                src/portvalidator.cpp \
                src/addprofiledialog.cpp \
                src/ssvalidator.cpp \
                src/ssprofile.cpp \
                src/configuration.cpp \
                src/qrwidget.cpp \
                src/sharedialog.cpp \
    $$PWD/editdialog.cpp \
    $$PWD/logdialog.cpp

HEADERS      += src/mainwindow.h \
                src/ss_process.h \
                src/ssprofile.h \
                src/ip4validator.h \
                src/portvalidator.h \
                src/addprofiledialog.h \
                src/ssvalidator.h \
                src/configuration.h \
                src/qrwidget.h \
                src/sharedialog.h \
    $$PWD/editdialog.h \
    $$PWD/logdialog.h

FORMS        += src/mainwindow.ui \
                src/addprofiledialog.ui \
                src/sharedialog.ui \
    $$PWD/editdialog.ui \
    $$PWD/logdialog.ui

RESOURCES    += src/icons.qrc \
                src/translations.qrc

TRANSLATIONS  = src/i18n/ss-qt5_zh_CN.ts

win32: RC_FILE = src/ss-qt5.rc
mac:   ICON    = src/ss-qt5.icns

isEmpty(BOTAN_VER) {
    BOTAN_VER = 1.10
}

win32: {
    win32-msvc*: error("Doesn't Support MSVC! Please use MinGW GCC.")
    else: {
        INCLUDEPATH +=  $$top_srcdir/3rdparty/qrencode/include \
                        $$top_srcdir/3rdparty/zbar/include \
                        D:/Projects/libQtShadowsocks/lib#just for convenience
        contains(DEFINES, mingw64): {
            LIBS += -L$$top_srcdir/3rdparty/qrencode/mingw64 \
                    -L$$top_srcdir/3rdparty/zbar/mingw64
        }
        else {
            LIBS += -L$$top_srcdir/3rdparty/qrencode/mingw32 \
                    -L$$top_srcdir/3rdparty/zbar/mingw32
        }
    }
    LIBS += -L./ -lqrencode -lQtShadowsocks -lbotan-$$BOTAN_VER -lzbar -liconv
}
unix : {
    CONFIG    += link_pkgconfig
    PKGCONFIG += libqrencode QtShadowsocks botan-$$BOTAN_VER zbar
    contains(DEFINES, UBUNTU_UNITY): {
        PKGCONFIG += gtk+-2.0 appindicator-0.1
    }
}
