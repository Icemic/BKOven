#-------------------------------------------------
#
# Project created by QtCreator 2015-02-15T11:42:34
#
#-------------------------------------------------

QT       += core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BKOven
TEMPLATE = app
VERSION = "0.0.0.1"


SOURCES += main.cpp\
        mainwindow.cpp \
    SozaiWidget/ImageSelectDialog.cpp \
    SozaiWidget/SoundSelectDialog.cpp \
    SozaiWidget/SozaiWidget.cpp \
    SozaiWidget/SozaiWidgetPrivate.cpp \
    StageWidget/StageWidget.cpp \
    StageWidget/StageWidgetPrivate.cpp \
    SozaiWidget/AddConfigDialog.cpp \
    ScriptWidget/ScriptWidget.cpp \
    ScriptWidget/ScriptWidgetPrivate.cpp \
    ScriptWidget/ScriptTreeWidget.cpp \
    ScriptWidget/ScriptListWidget.cpp \
    ScriptWidget/ScriptListWidgetItem.cpp \
    ScriptGeneration/ScriptGenerator.cpp

HEADERS  += mainwindow.h \
    SozaiWidget/ImageSelectDialog.h \
    SozaiWidget/SoundSelectDialog.h \
    SozaiWidget/SozaiWidget.h \
    SozaiWidget/SozaiWidgetPrivate.h \
    StageWidget/StageWidget.h \
    StageWidget/StageWidgetPrivate.h \
    SozaiWidget/AddConfigDialog.h \
    ScriptWidget/ScriptWidget.h \
    ScriptWidget/ScriptWidgetPrivate.h \
    ScriptWidget/ScriptTreeWidget.h \
    ScriptWidget/ScriptListWidget.h \
    ScriptWidget/ScriptListWidgetItem.h \
    stable.h \
    ScriptGeneration/ScriptGenerator.h

FORMS    += mainwindow.ui \
    SozaiWidget/SozaiWidgetPrivate.ui \
    StageWidget/StageWidgetPrivate.ui \
    SozaiWidget/AddConfigDialog.ui \
    ScriptWidget/ScriptWidgetPrivate.ui

CONFIG += c++11

PRECOMPILED_HEADER = stable.h

win32:!win32-g++:QMAKE_CXXFLAGS += /MP

RESOURCES += \
    SozaiWidget/icon.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ParserHelper/release/ -lParserHelper
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ParserHelper/debug/ -lParserHelper
else:unix: LIBS += -L$$OUT_PWD/../ParserHelper/ -lParserHelper

INCLUDEPATH += $$PWD/../ParserHelper
DEPENDPATH += $$PWD/../ParserHelper

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../BKAudio/release/ -lBKAudio
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../BKAudio/debug/ -lBKAudio
else:unix: LIBS += -L$$OUT_PWD/../BKAudio/ -lBKAudio

INCLUDEPATH += $$PWD/../BKAudio
DEPENDPATH += $$PWD/../BKAudio

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../shared/taglib/vs2013/lib/ -ltag
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../shared/taglib/vs2013/lib/ -ltag
else:unix: LIBS += -L$$PWD/../shared/taglib/vs2013/lib/ -ltag

INCLUDEPATH += $$PWD/../shared/taglib/vs2013/include
DEPENDPATH += $$PWD/../shared/taglib/vs2013/include

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
