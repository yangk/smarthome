#-------------------------------------------------
#
# Project created by QtCreator 2015-12-16T16:54:27
#
#-------------------------------------------------

QT       += widgets network serialport

TARGET = Comport
TEMPLATE = lib

builddir=../build/

CONFIG+=debug_and_release
CONFIG(debug,debug|release){
builddir=$${builddir}debug/
unix:TARGET=$$join(TARGET,,,_debug)
win32:TARGET=$$join(TARGET,,,d)
}else{
builddir=$${builddir}release/
}

DESTDIR = $${builddir}bin
UI_DIR  = $${builddir}forms
RCC_DIR = $${builddir}tmp
MOC_DIR = $${builddir}tmp
OBJECTS_DIR = $${builddir}tmp
DEFINES += COMPORT_LIBRARY

include(3rdparty/qextserialport/src/qextserialport.pri)

INCLUDEPATH += ../Commfunc

SOURCES += \
    Comportdialog.cpp

HEADERS +=\
    Comportdialog.h

LIBS+= -L $${DESTDIR}
Debug{
LIBS += -lCommFuncd
}
Release{
LIBS += -lCommFunc
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    Comportdialog.ui
