#-------------------------------------------------
#
# Project created by QtCreator 2015-12-16T17:06:49
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SmartHome
TEMPLATE = app

CONFIG += c++11

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

#PRECOMPILED_HEADER = stable.h

INCLUDEPATH +=../Comport ../About ../Commfunc

LIBS+= -L $${DESTDIR}
Debug{
LIBS += -lComportd -laboutd -lCommFuncd
}
Release{
LIBS += -lComport -labout -lCommFunc
}

SOURCES += main.cpp\
        mainwindow.cpp \
    SmartFrame.cpp \
    errorcode.cpp \
    hled.cpp \
    TaskMan.cpp \
    GlobalSetting.cpp \
    DevMan/DevManDialog.cpp \
    Database/ddm.cpp \
    CommApp/CommApp.cpp \
    DevMan/DevSetWidget.cpp \
    Tools/ToolAscStr.cpp \
    DevMan/ModemSetting.cpp \
    Tools/TestCase.cpp \
    CommWidget/CommWidget.cpp \
    CommApp/UpdateApp.cpp \
    CommApp/SuccessRatioApp.cpp \
    CommApp/BaseApp.cpp \
    CommApp/SendGroup.cpp

HEADERS  += mainwindow.h \
    SmartFrame.h \
    errorcode.h \
    hled.h \
    TaskMan.h \
    GlobalSetting.h \
    stable.h \
    DevMan/DevManDialog.h \
    Database/ddm.h \
    CommApp/CommApp.h \
    DevMan/DevSetWidget.h \
    Tools/ToolAscStr.h \
    DevMan/ModemSetting.h \
    Tools/TestCase.h \
    CommWidget/CommWidget.h \
    CommApp/UpdateApp.h \
    CommApp/SuccessRatioApp.h \
    CommApp/BaseApp.h \
    CommApp/SendGroup.h

FORMS    += mainwindow.ui \
    DevMan/DevManDialog.ui \
    DevMan/DevSetWidget.ui \
    DevMan/ModemSetDialog.ui \
    CommApp/SendGroupWidget.ui \
    CommApp/CommAppWidget.ui \
    CommApp/UpdateWidget.ui \
    Tools/ToolAscStr.ui \
    Tools/testcasedialog.ui

RESOURCES += \
    image.qrc
