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
