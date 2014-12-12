SOURCES      += src/main.cpp\
                src/mainwindow.cpp \
                src/ss_process.cpp \
                src/ip4validator.cpp \
                src/portvalidator.cpp \
                src/addprofiledialogue.cpp \
                src/ssvalidator.cpp \
                src/ssprofile.cpp \
                src/configuration.cpp \
                src/qrwidget.cpp \
                src/sharedialogue.cpp

HEADERS      += src/mainwindow.h \
                src/ss_process.h \
                src/ssprofile.h \
                src/ip4validator.h \
                src/portvalidator.h \
                src/addprofiledialogue.h \
                src/ssvalidator.h \
                src/configuration.h \
                src/qrwidget.h \
                src/sharedialogue.h

FORMS        += src/mainwindow.ui \
                src/addprofiledialogue.ui \
                src/sharedialogue.ui

RESOURCES    += src/icons.qrc

TRANSLATIONS  = src/i18n/ssqt5_zh_CN.ts

win32: RC_FILE = src/ss-qt5.rc
mac:   ICON    = src/ss-qt5.icns

win32: {
    win32-msvc*: error("Doesn't Support MSVC! Please use MinGW GCC.")
    else: {
        INCLUDEPATH +=  $$top_srcdir/3rdparty/qrencode/include
        contains(DEFINES, mingw64): {
            LIBS += -L$$top_srcdir/3rdparty/qrencode/mingw64
        }
        else {
            LIBS += -L$$top_srcdir/3rdparty/qrencode/mingw32
        }
    }
    LIBS += -lqrencode -lQtShadowsocks -lbotan-1.10
}
unix : {
    CONFIG    += link_pkgconfig
    PKGCONFIG += libqrencode QtShadowsocks botan-1.10
}
