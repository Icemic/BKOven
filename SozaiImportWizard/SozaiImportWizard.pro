#-------------------------------------------------
#
# Project created by QtCreator 2015-02-15T20:53:49
#
#-------------------------------------------------

QT       += core gui widgets opengl multimedia

TARGET = SozaiImportWizard
TEMPLATE = lib
CONFIG += staticlib

SOURCES += SozaiImportWizard.cpp \
    SozaiImportDialog.cpp

HEADERS += SozaiImportWizard.h \
    SozaiImportDialog.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    SozaiImportDialog.ui

CONFIG += c++11

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ParserHelper/release/ -lParserHelper
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ParserHelper/debug/ -lParserHelper
else:unix: LIBS += -L$$OUT_PWD/../ParserHelper/ -lParserHelper

INCLUDEPATH += $$PWD/../ParserHelper
DEPENDPATH += $$PWD/../ParserHelper
