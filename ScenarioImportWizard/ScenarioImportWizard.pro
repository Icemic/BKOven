#-------------------------------------------------
#
# Project created by QtCreator 2015-02-24T12:20:51
#
#-------------------------------------------------

QT       += widgets

TARGET = ScenarioImportWizard
TEMPLATE = lib
CONFIG += staticlib

SOURCES += ScenarioImportWizard.cpp

HEADERS += ScenarioImportWizard.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
