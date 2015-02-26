#-------------------------------------------------
#
# Project created by QtCreator 2015-02-15T11:42:34
#
#-------------------------------------------------

QT       += core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BKOven
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    SozaiWidget/ImageSelectDialog.cpp \
    SozaiWidget/SoundSelectDialog.cpp \
    SozaiWidget/SozaiWidget.cpp \
    SozaiWidget/SozaiWidgetPrivate.cpp

HEADERS  += mainwindow.h \
    SozaiWidget/ImageSelectDialog.h \
    SozaiWidget/SoundSelectDialog.h \
    SozaiWidget/SozaiWidget.h \
    SozaiWidget/SozaiWidgetPrivate.h

FORMS    += mainwindow.ui \
    SozaiWidget/SozaiWidgetPrivate.ui

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
