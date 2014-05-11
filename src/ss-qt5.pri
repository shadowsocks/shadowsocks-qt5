SOURCES      += src/main.cpp\
                src/mainwindow.cpp \
                src/ss_process.cpp \
                src/ip4validator.cpp \
                src/portvalidator.cpp \
                src/addprofiledialogue.cpp \
                src/ssvalidator.cpp \
                src/ssprofile.cpp \
    src/configuration.cpp

HEADERS      += src/mainwindow.h \
                src/ss_process.h \
                src/ssprofile.h \
                src/ip4validator.h \
                src/portvalidator.h \
                src/addprofiledialogue.h \
                src/ssvalidator.h \
    src/configuration.h

FORMS        += src/mainwindow.ui \
                src/addprofiledialogue.ui

RESOURCES    += src/icons.qrc

TRANSLATIONS  = src/i18n/ssqt5_zh_CN.ts

win32: RC_FILE = src/ss-qt5.rc
mac:   ICON    = src/ss-qt5.icns
