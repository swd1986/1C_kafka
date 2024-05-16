QT -= gui
#QT += serialport
TEMPLATE = lib
DEFINES += TTYADDIN_LIBRARY
CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH +="/home/delphin/ProjectQt/estern_lib2/include/"

SOURCES += \
    Component.cpp \
    dllmain.cpp \
    exports.cpp \
    ttyaddin.cpp

HEADERS += \
    Component.h \
    stdafx.h \
    ttyAddin_global.h \
    ttyaddin.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
