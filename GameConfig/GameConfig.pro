#-------------------------------------------------
#
# Project created by QtCreator 2015-03-09T20:45:57
#
#-------------------------------------------------

QT       += widgets

TARGET = GameConfig
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    GameConfig.cpp \

HEADERS += \
    GameConfig.h \

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    GameConfig.ui \

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ParserHelper/release/ -lParserHelper
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ParserHelper/debug/ -lParserHelper
else:unix: LIBS += -L$$OUT_PWD/../ParserHelper/ -lParserHelper

INCLUDEPATH += $$PWD/../ParserHelper
DEPENDPATH += $$PWD/../ParserHelper
