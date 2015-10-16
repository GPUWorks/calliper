#-------------------------------------------------
#
# Project created by QtCreator 2015-09-27T21:39:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = calliper
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    cviewport.cpp \
    cbasiccamera.cpp \
    ccameralens.cpp \
    csceneobject.cpp \
    callipermath.cpp \
    ceulerangle.cpp

HEADERS  += mainwindow.h \
    cviewport.h \
    cbasiccamera.h \
    ccameralens.h \
    csceneobject.h \
    callipermath.h \
    ceulerangle.h

FORMS    += mainwindow.ui

RESOURCES += \
    resource.qrc
