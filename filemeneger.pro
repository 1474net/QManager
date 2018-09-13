#-------------------------------------------------
#
# Project created by QtCreator 2015-11-27T10:30:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = filemeneger
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    pane.cpp \
    properties.cpp

HEADERS  += mainwindow.h \
    pane.h \
    properties.h

FORMS    += mainwindow.ui

RESOURCES += \
    resource.qrc
