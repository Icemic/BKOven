#-------------------------------------------------
#
# Project created by QtCreator 2015-03-06T22:40:16
#
#-------------------------------------------------

TARGET = BKFontText
TEMPLATE = lib

CONFIG += c++11

DEFINES += BKFONTTEXT_LIBRARY

SOURCES += \
    Font.cpp \
    CCObject.cpp \
    ImageSupport.cpp \
    BKFontText.cpp

HEADERS += \
        bkfonttext_global.h \
    Font.h \
    BasicCache.h \
    CCObject.h \
    ImageSupport.h \
    BKFontText.h

LIBS += $$PWD/freetype.lib

INCLUDEPATH += $$PWD/

unix {
    target.path = /usr/lib
    INSTALLS += target
}
