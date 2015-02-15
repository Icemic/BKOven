#-------------------------------------------------
#
# Project created by QtCreator 2015-02-15T20:53:49
#
#-------------------------------------------------

QT       += widgets opengl multimedia

TARGET = SozaiImportWizard
TEMPLATE = lib
CONFIG += staticlib

SOURCES += SozaiImportWizard.cpp

HEADERS += SozaiImportWizard.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
