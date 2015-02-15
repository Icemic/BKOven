#-------------------------------------------------
#
# Project created by QtCreator 2015-02-15T13:37:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NewProjectWizard
TEMPLATE = lib
CONFIG += staticlib

SOURCES += NewProjectWizard.cpp \
    NewProjectDialog.cpp

HEADERS += NewProjectWizard.h \
    NewProjectDialog.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    NewProjectDialog.ui

CONFIG += c++11

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ParserHelper/release/ -lParserHelper
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ParserHelper/debug/ -lParserHelper
else:unix: LIBS += -L$$OUT_PWD/../ParserHelper/ -lParserHelper

INCLUDEPATH += $$PWD/../ParserHelper
DEPENDPATH += $$PWD/../ParserHelper
