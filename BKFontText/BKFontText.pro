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
    BKFontText.h \
    Common.h \
    ft2build.h \
    freetype/config/ftconfig.h \
    freetype/config/ftheader.h \
    freetype/config/ftmodule.h \
    freetype/config/ftoption.h \
    freetype/config/ftstdlib.h \
    freetype/internal/services/svbdf.h \
    freetype/internal/services/svcid.h \
    freetype/internal/services/svgldict.h \
    freetype/internal/services/svgxval.h \
    freetype/internal/services/svkern.h \
    freetype/internal/services/svmm.h \
    freetype/internal/services/svotval.h \
    freetype/internal/services/svpfr.h \
    freetype/internal/services/svpostnm.h \
    freetype/internal/services/svprop.h \
    freetype/internal/services/svpscmap.h \
    freetype/internal/services/svpsinfo.h \
    freetype/internal/services/svsfnt.h \
    freetype/internal/services/svttcmap.h \
    freetype/internal/services/svtteng.h \
    freetype/internal/services/svttglyf.h \
    freetype/internal/services/svwinfnt.h \
    freetype/internal/services/svxf86nm.h \
    freetype/internal/autohint.h \
    freetype/internal/ftcalc.h \
    freetype/internal/ftdebug.h \
    freetype/internal/ftdriver.h \
    freetype/internal/ftgloadr.h \
    freetype/internal/ftmemory.h \
    freetype/internal/ftobjs.h \
    freetype/internal/ftpic.h \
    freetype/internal/ftrfork.h \
    freetype/internal/ftserv.h \
    freetype/internal/ftstream.h \
    freetype/internal/fttrace.h \
    freetype/internal/ftvalid.h \
    freetype/internal/internal.h \
    freetype/internal/psaux.h \
    freetype/internal/pshints.h \
    freetype/internal/sfnt.h \
    freetype/internal/t1types.h \
    freetype/internal/tttypes.h \
    freetype/freetype.h \
    freetype/ftadvanc.h \
    freetype/ftautoh.h \
    freetype/ftbbox.h \
    freetype/ftbdf.h \
    freetype/ftbitmap.h \
    freetype/ftbzip2.h \
    freetype/ftcache.h \
    freetype/ftcffdrv.h \
    freetype/ftchapters.h \
    freetype/ftcid.h \
    freetype/fterrdef.h \
    freetype/fterrors.h \
    freetype/ftgasp.h \
    freetype/ftglyph.h \
    freetype/ftgxval.h \
    freetype/ftgzip.h \
    freetype/ftimage.h \
    freetype/ftincrem.h \
    freetype/ftlcdfil.h \
    freetype/ftlist.h \
    freetype/ftlzw.h \
    freetype/ftmac.h \
    freetype/ftmm.h \
    freetype/ftmodapi.h \
    freetype/ftmoderr.h \
    freetype/ftotval.h \
    freetype/ftoutln.h \
    freetype/ftpfr.h \
    freetype/ftrender.h \
    freetype/ftsizes.h \
    freetype/ftsnames.h \
    freetype/ftstroke.h \
    freetype/ftsynth.h \
    freetype/ftsystem.h \
    freetype/fttrigon.h \
    freetype/ftttdrv.h \
    freetype/fttypes.h \
    freetype/ftwinfnt.h \
    freetype/ftxf86.h \
    freetype/t1tables.h \
    freetype/ttnameid.h \
    freetype/tttables.h \
    freetype/tttags.h \
    freetype/ttunpat.h

LIBS += $$PWD/freetype.lib

INCLUDEPATH += $$PWD/

unix {
    target.path = /usr/lib
    INSTALLS += target
}

CONFIG += c++11
