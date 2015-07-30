#-------------------------------------------------
#
# Project created by QtCreator 2015-03-06T17:09:52
#
#-------------------------------------------------

QT       += widgets

TARGET = PreviewWidget
TEMPLATE = lib

DEFINES += PREVIEWWIDGET_LIBRARY

SOURCES += PreviewWidget.cpp \
    PreviewScene.cpp

HEADERS += PreviewWidget.h\
        previewwidget_global.h \
    PreviewScene.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../BKFontText/release/ -lBKFontText
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../BKFontText/debug/ -lBKFontText
else:unix: LIBS += -L$$OUT_PWD/../BKFontText/ -lBKFontText

INCLUDEPATH += $$PWD/../BKFontText
DEPENDPATH += $$PWD/../BKFontText

CONFIG += c++11
