#-------------------------------------------------
#
# Project created by QtCreator 2015-12-17T09:30:36
#
#-------------------------------------------------

QT       -= gui

TARGET = CommFunc
TEMPLATE = lib

builddir=../build/

CONFIG += debug_and_release
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

DEFINES += COMMFUNC_LIBRARY

SOURCES += CommFunc.cpp \
    fifo.c \
    LogMan.cpp

HEADERS += CommFunc.h\
    types.h \
    list.h \
    fifo.h \
    LogMan.h
