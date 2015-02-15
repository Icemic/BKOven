#-------------------------------------------------
#
# Project created by QtCreator 2015-02-15T11:47:30
#
#-------------------------------------------------

QT       += widgets

TARGET = NewProjectWizard
TEMPLATE = lib
CONFIG += staticlib

SOURCES += NewProjectWizard.cpp

HEADERS += NewProjectWizard.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
