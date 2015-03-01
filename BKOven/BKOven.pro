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
    ScriptWidget/ScriptListWidgetItem.cpp

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
    ScriptWidget/ScriptListWidgetItem.h

FORMS    += mainwindow.ui \
    SozaiWidget/SozaiWidgetPrivate.ui \
    StageWidget/StageWidgetPrivate.ui \
    SozaiWidget/AddConfigDialog.ui \
    ScriptWidget/ScriptWidgetPrivate.ui

CONFIG += c++11

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
