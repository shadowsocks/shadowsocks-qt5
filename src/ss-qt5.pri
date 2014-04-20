SOURCES   += src/main.cpp\
             src/mainwindow.cpp \
             src/profiles.cpp \
             src/ss_process.cpp \
             src/ip4validator.cpp \
             src/portvalidator.cpp

HEADERS   += src/mainwindow.h \
             src/profiles.h \
             src/ss_process.h \
             src/ssprofile.h \
             src/ip4validator.h \
             src/portvalidator.h

FORMS     += src/mainwindow.ui

RESOURCES += src/icons.qrc

win32: RC_FILE = src/ss-qt5.rc
mac:   ICON    = src/ss-qt5.icns