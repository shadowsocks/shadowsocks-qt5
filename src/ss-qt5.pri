SOURCES      += src/main.cpp\
                src/mainwindow.cpp \
                src/ip4validator.cpp \
                src/portvalidator.cpp \
                src/ssvalidator.cpp \
                src/qrwidget.cpp \
                src/sharedialog.cpp \
                src/editdialog.cpp \
                src/logdialog.cpp \
                src/connection.cpp \
                src/confighelper.cpp \
                src/urihelper.cpp \
                src/uriinputdialog.cpp \
                src/sqprofile.cpp \
                src/settingsdialog.cpp \
                src/statusdialog.cpp \
                src/controllerthread.cpp \
                src/statusnotifier.cpp

HEADERS      += src/mainwindow.h \
                src/ip4validator.h \
                src/portvalidator.h \
                src/ssvalidator.h \
                src/qrwidget.h \
                src/sharedialog.h \
                src/editdialog.h \
                src/logdialog.h \
                src/connection.h \
                src/confighelper.h \
                src/urihelper.h \
                src/uriinputdialog.h \
                src/sqprofile.h \
                src/settingsdialog.h \
                src/statusdialog.h \
                src/controllerthread.h \
                src/statusnotifier.h

FORMS        += src/mainwindow.ui \
                src/sharedialog.ui \
                src/editdialog.ui \
                src/logdialog.ui \
                src/uriinputdialog.ui \
                src/settingsdialog.ui \
                src/statusdialog.ui

RESOURCES    += src/icons.qrc \
                src/translations.qrc

TRANSLATIONS  = src/i18n/ss-qt5_zh_CN.ts

win32: RC_FILE = src/ss-qt5.rc

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
    PKGCONFIG += libqrencode QtShadowsocks botan-$$BOTAN_VER zbar gtk+-2.0 appindicator-0.1
}
