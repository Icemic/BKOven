#-------------------------------------------------
#
# Project created by QtCreator 2015-02-15T13:40:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Test
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

CONFIG += c++11

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../NewProjectWizard/release/ -lNewProjectWizard
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../NewProjectWizard/debug/ -lNewProjectWizard
else:unix: LIBS += -L$$OUT_PWD/../NewProjectWizard/ -lNewProjectWizard

INCLUDEPATH += $$PWD/../NewProjectWizard
DEPENDPATH += $$PWD/../NewProjectWizard

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../NewProjectWizard/release/libNewProjectWizard.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../NewProjectWizard/debug/libNewProjectWizard.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../NewProjectWizard/release/NewProjectWizard.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../NewProjectWizard/debug/NewProjectWizard.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../NewProjectWizard/libNewProjectWizard.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../SozaiImportWizard/release/ -lSozaiImportWizard
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../SozaiImportWizard/debug/ -lSozaiImportWizard
else:unix: LIBS += -L$$OUT_PWD/../SozaiImportWizard/ -lSozaiImportWizard

INCLUDEPATH += $$PWD/../SozaiImportWizard
DEPENDPATH += $$PWD/../SozaiImportWizard

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../SozaiImportWizard/release/libSozaiImportWizard.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../SozaiImportWizard/debug/libSozaiImportWizard.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../SozaiImportWizard/release/SozaiImportWizard.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../SozaiImportWizard/debug/SozaiImportWizard.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../SozaiImportWizard/libSozaiImportWizard.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ParserHelper/release/ -lParserHelper
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ParserHelper/debug/ -lParserHelper
else:unix: LIBS += -L$$OUT_PWD/../ParserHelper/ -lParserHelper

INCLUDEPATH += $$PWD/../ParserHelper
DEPENDPATH += $$PWD/../ParserHelper
