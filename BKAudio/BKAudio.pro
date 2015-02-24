#-------------------------------------------------
#
# Project created by QtCreator 2015-02-24T10:05:59
#
#-------------------------------------------------

QT       -= gui

CONFIG += c++11 hidesyms

TARGET = BKAudio
TEMPLATE = lib

DEFINES += BKAUDIO_LIBRARY

SOURCES += Audio.cpp \
    AudioFormats.cpp \
    OpenAL.cpp \
    Thread.cpp \
    vorbis/analysis.c \
    vorbis/bitrate.c \
    vorbis/bitwise.c \
    vorbis/block.c \
    vorbis/codebook.c \
    vorbis/envelope.c \
    vorbis/floor0.c \
    vorbis/floor1.c \
    vorbis/framing.c \
    vorbis/info.c \
    vorbis/lookup.c \
    vorbis/lpc.c \
    vorbis/lsp.c \
    vorbis/mapping0.c \
    vorbis/mdct.c \
    vorbis/psy.c \
    vorbis/registry.c \
    vorbis/res0.c \
    vorbis/sharedbook.c \
    vorbis/smallft.c \
    vorbis/synthesis.c \
    vorbis/vorbisenc.c \
    vorbis/vorbisfile.c \
    vorbis/window.c \
    BKAudio.cpp \
    mp3/AbstractLayer.cpp \
    mp3/BitStream.cpp \
    mp3/BitStreamMainData.cpp \
    mp3/ChannelInformation.cpp \
    mp3/Header.cpp \
    mp3/Layer3.cpp \
    mp3/MP3Decoder.cpp \
    mp3/Synthesis2.cpp

HEADERS +=\
        bkaudio_global.h \
    Audio.h \
    AudioFormats.h \
    OpenAL.h \
    Thread.h \
    Common.h \
    vorbis/books/coupled/res_books_51.h \
    vorbis/books/coupled/res_books_stereo.h \
    vorbis/books/floor/floor_books.h \
    vorbis/books/uncoupled/res_books_uncoupled.h \
    vorbis/modes/floor_all.h \
    vorbis/modes/psych_11.h \
    vorbis/modes/psych_16.h \
    vorbis/modes/psych_44.h \
    vorbis/modes/psych_8.h \
    vorbis/modes/residue_16.h \
    vorbis/modes/residue_44.h \
    vorbis/modes/residue_44p51.h \
    vorbis/modes/residue_44u.h \
    vorbis/modes/residue_8.h \
    vorbis/modes/setup_11.h \
    vorbis/modes/setup_16.h \
    vorbis/modes/setup_22.h \
    vorbis/modes/setup_32.h \
    vorbis/modes/setup_44.h \
    vorbis/modes/setup_44p51.h \
    vorbis/modes/setup_44u.h \
    vorbis/modes/setup_8.h \
    vorbis/modes/setup_X.h \
    vorbis/backends.h \
    vorbis/bitrate.h \
    vorbis/codebook.h \
    vorbis/codec_internal.h \
    vorbis/envelope.h \
    vorbis/highlevel.h \
    vorbis/lookup.h \
    vorbis/lookup_data.h \
    vorbis/lpc.h \
    vorbis/lsp.h \
    vorbis/masking.h \
    vorbis/mdct.h \
    vorbis/misc.h \
    vorbis/os.h \
    vorbis/psy.h \
    vorbis/registry.h \
    vorbis/scales.h \
    vorbis/smallft.h \
    vorbis/window.h \
    BKAudio.h \
    mp3/AbstractLayer.h \
    mp3/BitStream.h \
    mp3/BitStreamMainData.h \
    mp3/ChannelInformation.h \
    mp3/Header.h \
    mp3/IReader.h \
    mp3/Layer3.h \
    mp3/MP3Decoder.h \
    mp3/Synthesis.h

INCLUDEPATH += vorbis

QMAKE_CXXFLAGS += -fvisibility=hidden
QMAKE_CFLAGS += -fvisibility=hidden

win32{
    INCLUDEPATH += include
    LIBS += -L$${PWD}/lib/win -lOpenAL32 -s
}

