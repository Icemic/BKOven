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
    extend/QLineEditEx.cpp

HEADERS += \
    GameConfig.h \
    extend/QLineEditEx.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../PreviewWidget/release/ -lPreviewWidget
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../PreviewWidget/debug/ -lPreviewWidget
else:unix: LIBS += -L$$OUT_PWD/../PreviewWidget/ -lPreviewWidget

INCLUDEPATH += $$PWD/../PreviewWidget
DEPENDPATH += $$PWD/../PreviewWidget
