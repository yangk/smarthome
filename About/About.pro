#-------------------------------------------------
#
# Project created by QtCreator 2012-06-10T09:37:48
#
#-------------------------------------------------
QT       += widgets

TARGET = about
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

DEFINES += ABOUT_LIBRARY

SOURCES += about.cpp

HEADERS += about.h

FORMS += \
    dialog.ui

RESOURCES += \
    resource.qrc
