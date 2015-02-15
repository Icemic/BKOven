#-------------------------------------------------
#
# Project created by QtCreator 2014-10-15T16:55:40
#
#-------------------------------------------------

QT       -= gui
QT       += core


TARGET = ParserHelper
TEMPLATE = lib

DEFINES += PARSERHELPER_LIBRARY

SOURCES += ParserHelper.cpp \
    parser/BKE_hash.cpp \
    parser/BKE_number.cpp \
    parser/BKE_string.cpp \
    parser/BKE_variable.cpp \
    parser/extend.cpp \
    parser/parser.cpp \
    parser/utils.cpp

HEADERS += ParserHelper.h\
        parserhelper_global.h \
    parser/BKE_array.h \
    parser/BKE_hash.hpp \
    parser/BKE_number.h \
    parser/BKE_string.h \
    parser/BKE_variable.h \
    parser/extend.h \
    parser/parser.h \
    parser/utils.h \
    parser/vcode.h \
    config.h

win32-g++:QMAKE_CFLAGS += -Wunused-parameter
win32-g++:QMAKE_CXXFLAGS += -Wunused-parameter

unix {
    target.path = /usr/lib
    INSTALLS += target
}


win32-g++:QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter -Wno-unused-variable

CONFIG += c++11
