#pragma once

#include "Common.h"
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftoutln.h>
#include <freetype/ftstroke.h>
#include <freetype/ftglyph.h>
#include <freetype/ttnameid.h>
#include <freetype/ftsystem.h>

#include "BasicCache.h"
#include "CCObject.h"

#include <QtCore>
#include <QtGui>

#include "ImageSupport.h"

typedef unsigned char byte;
#define x x
#define QString QString

class BKE_Glyph;
class BKE_Sprite;

#define FT_FLOOR(X)	((X & -64) >> 6)
#define FT_CEIL(X)	(((X + 63) & -64) >> 6)

enum bkeStyle
{
	bkeStyleNormal = 0x00,
	bkeStyleBlod = 0x01,
	bkeStyleItalic = 0x02,
	bkeStyleUnderline = 0x04,
	bkeStyleStrikethrough = 0x08
};

class BKE_Font;
class BKE_Variable;
//大小固定为font->height * glyph->width, 经过颜色处理
class BKE_Glyph : public CCObject
{
    void _drawGlyph(FT_Glyph glyph, QByteArray &buf, int &bufwidth, int &bufheight);
	bool good;
public:
    QByteArray bitmap;
	int height;
	int width;
	int advancex;
	int advancey;
	BKE_Font *font;

	int left, top;

	BKE_Glyph(bkushort ch, BKE_Font *f);
    inline void drawTo(QByteArray &data, bkulong dstWidth, bkulong dstHeight, bklong x, bklong y)
	{
		drawAt(data, dstWidth, dstHeight, bitmap, this->width, this->height, x + left, y + top);
	}
    inline void drawTo(QByteArray &data, bkulong dstWidth, bkulong dstHeight, bklong x, bklong y, bkulong width, bkulong height)
    {
        drawAt(data, dstWidth, dstHeight, bitmap, width, height, x + left, y + top);
    }
	inline int getActualWidth(){ return left + width; }
	~BKE_Glyph(){}
	bool operator !() const { return !good; }
};

struct BKE_FontColor
{
    QColor from;
    QColor to;
    BKE_FontColor() :BKE_FontColor(QColor(0, 0, 0, 0)){}
    BKE_FontColor(const QColor &c) :from(c), to(c){}
    BKE_FontColor(const QColor &a, const QColor &b) :from(a), to(b){}

    void set(const QColor &c){ from = c; to = c; }
    void set(const QColor &a, const QColor &b){ from = a; to = b; }

    bool isSingleColor() const { return from == to; }

	inline bool operator == (const BKE_FontColor &r) const {
        return from == r.from && to == r.to;
	}
};

struct BKE_FontStyleInfo
{
	bkulong style;
	BKE_FontColor color;
	bool enableshadow;
    QColor shadow;
	bool enablestroke;
    QColor stroke;
	BKE_FontStyleInfo() :style(0), enableshadow(false), shadow({ 0, 0, 0, 255 }), enablestroke(false), stroke({ 0, 0, 0, 255 }){}

	inline bool operator == (const BKE_FontStyleInfo &r) const {
		return style == r.style && color == r.color && enableshadow == r.enableshadow && shadow == r.shadow && enablestroke == r.enablestroke && stroke == r.stroke;
	}
};

class BKE_Font : public CCObject
{
	typedef BKE_BasicCache<bkushort, BKE_Glyph *> glyphcache_t;
	vector<pair<BKE_FontStyleInfo, glyphcache_t *>> cache_glyphimage;
	friend bool operator ==(const pair<BKE_FontStyleInfo, glyphcache_t*> &l, const BKE_FontStyleInfo &r)
	{ 
		return l.first == r; 
	}
	glyphcache_t * cache_glyphimage_current;
	BKE_FontStyleInfo style;

	friend class BKE_Glyph;
	FT_Face face;
	FT_StreamRec stream;
    QFile fp;
	bool initialized;
public:
	//字体大小
	bklong height;
	bklong size;
	bklong ascent;
	bklong descent;
	bklong lineskip;
	bklong underline_offset;
	bklong underline_height;
    QString name;

	inline bool is_bold(){return !!(this->style.style & bkeStyleBlod);}
	inline bool is_italic(){ return !!(this->style.style & bkeStyleItalic); }
	inline bool is_underline(){ return !!(this->style.style & bkeStyleUnderline); }
	inline bool is_strike(){ return !!(this->style.style & bkeStyleStrikethrough); }

	BKE_Glyph* getGlyph(bkushort ch);
	void setStyle(const BKE_FontStyleInfo &i);
	BKE_FontStyleInfo getStyle() const { return style; }

    BKE_Font(FT_Library l, const QString &filename, bklong height);
	~BKE_Font();
	operator bool() const { return initialized; }
};

//ulong Font_Read(FT_Stream s,ulong offset,uchar *buffer,ulong count);
struct FontKey
{
    QString name;
	bklong size;

	FontKey(){};
    FontKey(const QString &n, bklong s) :name(n), size(s){};

	inline bool operator == (const FontKey &k) const
	{
		return name == k.name && size == k.size;
	}

    inline bool operator < (const FontKey &r) const
    {
        if(name < r.name)
            return true;
        else if(name == r.name)
        {
            if(size < r.size)
                return true;
        }
        return false;
    }
};

class BKE_FontInfo
{
	bool fontdirty = 0;
	BKE_Font *font = NULL;
#define FONT_INFO_PROPERTY_DIRTY(_t, _n, _v) private: _t m_##_n = _v; public: inline void set##_n(const _t &v){if(m_##_n!=v){m_##_n = v; fontdirty=true;}} inline _t get##_n() const {return m_##_n;}
#define FONT_INFO_PROPERTY(_t, _n, _v) private: _t m_##_n = _v; public: inline void set##_n(const _t &v){m_##_n = v;} inline _t get##_n() const {return m_##_n;}
	
    FONT_INFO_PROPERTY_DIRTY(QString, FontName, "");
	FONT_INFO_PROPERTY_DIRTY(bklong, FontSize, 24);
	
	FONT_INFO_PROPERTY(bklong, XInterval, 0);
	FONT_INFO_PROPERTY(bklong, YInterval, 12);

#undef FONT_INFO_PROPERTY_DIRTY
#undef FONT_INFO_PROPERTY

private:
	BKE_FontStyleInfo style;

#define FONT_INFO_PROPERTY_STYLE_WITHBOOL(_t, _n, _v) public: inline void set##_n(const _t &v){style._v = v;} inline _t get##_n() const {return style._v;} inline void set##_n##Enabled(bool b){style.enable##_v=b;} inline bool is##_n##Enabled(){return style.enable##_v;}
#define FONT_INFO_PROPERTY_STYLE(_t, _n, _v) public: inline void set##_n(const _t &v){style._v = v;} inline _t get##_n() const {return style._v;}
	
	FONT_INFO_PROPERTY_STYLE(bkulong, FontStyle, style);
	FONT_INFO_PROPERTY_STYLE(BKE_FontColor, FontColor, color);

    FONT_INFO_PROPERTY_STYLE_WITHBOOL(QColor, Shadow, shadow);
    FONT_INFO_PROPERTY_STYLE_WITHBOOL(QColor, Stroke, stroke);

#undef FONT_INFO_PROPERTY_STYLE
#undef FONT_INFO_PROPERTY_STYLE_WITHBOOL

public:
	BKE_FontInfo(){}
	BKE_FontInfo(const BKE_FontInfo &r) :fontdirty(r.fontdirty), font(r.font), m_FontName(r.m_FontName), m_FontSize(r.m_FontSize), m_XInterval(r.m_XInterval), m_YInterval(r.m_YInterval), style(r.style)
    {  if(font) font->retain(); }
    BKE_FontInfo &operator = (const BKE_FontInfo &r){ this->fontdirty = r.fontdirty; if(r.font) r.font->retain(); if(this->font) this->font->release(); this->font = r.font; this->m_FontName = r.m_FontName; this->m_FontSize = r.m_FontSize; this->m_XInterval = r.m_XInterval; this->m_YInterval = r.m_YInterval; this->style = r.style; return *this; }
	inline BKE_Font *getFont() const;
    inline void closeFont(){ if(font) {font->release(); font=NULL;} fontdirty = true; } //释放持有的Font句柄，防止内存被一直占用
    ~BKE_FontInfo(){ if(font) font->release(); }
	bool operator !() const { return !this->getFont(); }
};

class BKE_FontCache
{
	friend class BKE_Script;
private:
    static QMap<FontKey, BKE_Font*> fontcache;
#if defined(WIN32) || defined(LINUX) || defined(MACOS) 
	static const bklong filecachelimit = 60 * 1024 * 1024;
#else
	static const bklong filecachelimit = 30 * 1024 * 1024;;
#endif
	static bklong filecachesize;

	static FT_Library lib;

	static BKE_FontInfo default_info;
	static void GCFont();
    static BKE_Font* openFont(const QString &filename, bklong height);
    static BKE_Font* newFont(const QString &filename, bklong height);

public:
	static bool isinited;

	static bool initTTF();
    static QImage getGlyphImage(const QString &text, const BKE_FontInfo &info);
    static QImage getGlyphImageEx(const QString &text, const BKE_FontInfo &info, const QString &extraChar = "", bklong w = -1, bklong h = -1/*actually h is maxline*/);
	static bool setSystemFont(bklong size = 24, bkulong style = 0);
    static void setDefaultFont(const QString &name, bklong size, const BKE_FontColor &color);
	static inline BKE_FontInfo getDefaultFontInfo(){ return default_info; }
	//return drawed length , 0 if forcedraw==false && need reline
    static bklong drawChar(bkushort ch, QByteArray &data, bklong x, bklong y, int totalwidth, int totalheight, const BKE_FontInfo &info, bool forcedraw = false);
    static void getStringSizeWithLimit(const QString &text, bklong &width, bklong &height, bklong &lines, bklong limit, const BKE_FontInfo &info);
    static void getStringSize(const QString &text, bklong &width, bklong &height, const BKE_FontInfo &info);
    static bklong getStringSizeEx(const QString &text, bklong &width, bklong &height, const QString &extraChar, bklong &maxline, const BKE_FontInfo &info);
	
	static void uninit();
    static bklong getCharCount(bklong length, const QString &str, const BKE_FontInfo &info);

	friend class BKE_Font;
	friend class BKE_Glyph;
	friend class BKE_FontInfo;
};

inline BKE_Font *BKE_FontInfo::getFont() const
{
	if (fontdirty)
	{
        if(font)
            font->release();
		const_cast<BKE_Font *&>(font) = BKE_FontCache::openFont(m_FontName, m_FontSize);
		if (font)
		{
			font->retain();
		}
		const_cast<bool &>(fontdirty) = false;
	}
	if (font)
	{
		font->setStyle(style);
		return font;
	}
	return NULL;
}
