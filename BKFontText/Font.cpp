#include "Font.h"
#include "../shared/vshack.h"
#include <QDebug>

bool stdioSeek(QIODevice *stream, bklong offset, int direction){
    switch (direction){
    case SEEK_CUR:
        offset += stream->pos();
        break;
    case SEEK_END:
        offset = stream->size() - offset;
        break;
    case SEEK_SET:
        break;
    default:
        break;
    }
    return stream->seek(offset);
}


static unsigned long ft_stream_io(FT_Stream stream, unsigned long offset, unsigned char*  buffer, unsigned long count)
{
    QIODevice *s = static_cast<QIODevice *>(stream->descriptor.pointer);
    stdioSeek(s, offset, SEEK_SET);
	if (!count)
	{
		return 0;
	}
    return s->read((char*)buffer, count);
}

static void ft_stream_close(FT_Stream  stream)
{
    QIODevice *s = static_cast<QIODevice *>(stream->descriptor.pointer);
	s->close();
}

BKE_Font::BKE_Font(FT_Library l, const QString &filename, bklong height)
    :fp(filename)
{
	face = NULL;
	initialized = false;
    if(!fp.open(QFile::ReadOnly))
        return;
	memset(&stream, 0, sizeof(stream));
	FT_StreamDesc desc;
    desc.pointer = &fp;

	stream.descriptor = desc;
    stream.size = fp.size();
	stream.read = &ft_stream_io;
	stream.close = &ft_stream_close;
	FT_Open_Args args = { 0 };
	args.flags = FT_OPEN_STREAM;
	args.stream = &stream;
	FT_Error error = FT_Open_Face(l, &args, 0, &face);
	if (error)
	{
		return;
	}
	if (FT_IS_SCALABLE(face))
	{
		//字体可以改变大小
		error = FT_Set_Pixel_Sizes(face, height, 0);
		//error = FT_Set_Char_Size(face, 0, height << 6, 0, 0);
		if (error)
		{
			return;
		}
		FT_Fixed scale = face->size->metrics.y_scale;
		ascent = FT_CEIL(face->size->metrics.ascender);
		descent = FT_CEIL(face->size->metrics.descender);
// 		bklong as2 = FT_CEIL(FT_MulFix(face->bbox.yMax * face->units_per_EM / 2048, scale));
// 		bklong ds2 = FT_CEIL(FT_MulFix(face->bbox.yMin * face->units_per_EM / 2048, scale));
// 		if (as2 > ascent)
// 			ascent = as2;
// 		if (ds2 < descent)
// 			descent = ds2;
		//ascent = FT_CEIL(FT_MulFix(face->size->metrics.ascender, scale));
		//descent = FT_CEIL(FT_MulFix(face->size->metrics.descender, scale));
		this->height = ascent - descent + /* baseline */ 0;
		//this->height = FT_CEIL(FT_MulFix((face->bbox.yMax - face->bbox.yMin), scale));
		size = height;
		//lineskip = size;
		lineskip = FT_CEIL(FT_MulFix(face->height, scale));
		underline_offset = FT_FLOOR(FT_MulFix(face->underline_position, scale));
		underline_height = FT_FLOOR(FT_MulFix(face->underline_thickness, scale));
	}
	else
	{
		//字体只包含固定大小
		//此时若指定大小无法被使用则用默认大小
		int index = face->num_fixed_sizes;
		for (index--; index >= 0; index++)
			if (face->available_sizes[index].height == height)
				break;
		error = FT_Set_Pixel_Sizes(face, face->available_sizes[index].height, face->available_sizes[index].width);
		ascent = face->available_sizes[index].height;
		descent = 0;
		this->height = face->available_sizes[index].height;
		lineskip = FT_CEIL(ascent);
		underline_offset = FT_FLOOR(face->underline_position);
		underline_height = FT_FLOOR(face->underline_thickness);
	}
	if (underline_height < 1) {
		underline_height = 1;
	}
	initialized = true;
	setStyle(style);
	//拿255 ÿ来测试
	auto g = this->getGlyph(255);
	if (g->top + g->height >= this->height)
	{
		this->descent -= g->top + g->height - this->height;
		this->height = this->ascent - this->descent;
	}
};

BKE_Font::~BKE_Font()
{
	for (auto i : cache_glyphimage)
	{
		delete i.second;
	}
	if (BKE_FontCache::isinited)
		FT_Done_Face(this->face);
}

BKE_Glyph* BKE_Font::getGlyph(bkushort ch)
{
	// 	auto it = cache_glyph.find(ch);
	//  	if(it!=cache_glyph.end())
	// 		return &it->value;
	//不可打印字符直接返回NULL，以防一些2B字体返回框框
	if (ch < 32)
		return NULL;
	BKE_Glyph *g = NULL;
	if (cache_glyphimage_current->get(ch, g) && g)
	{
		//o_stdout << L"Existed Font:" << (wchar_t)ch << L'\n';
		return g;
	}
	g = new BKE_Glyph(ch, this);
	if (!*g)
	{
		delete g;
		return NULL;
	}
	//o_stdout << L"New Font:" << (wchar_t)ch << L'\n';
	cache_glyphimage_current->add(ch, g, 1);
	g->release();
	return g;
}

void BKE_Font::setStyle(const BKE_FontStyleInfo &i)
{
	glyphcache_t * cache;
	{
		auto it = cache_glyphimage.begin();
        for(; it != cache_glyphimage.end(); ++it)
            if(it->first == i)
                break;
		if (it == cache_glyphimage.end()) // 缓存不存在
		{
			if (cache_glyphimage.size()>3)
			{
				auto p = cache_glyphimage.back();
				delete p.second;
				cache_glyphimage.pop_back();
			}
			cache = new glyphcache_t();
			cache->limit(256);
			cache_glyphimage.insert(cache_glyphimage.begin(), { i, cache });
		}
		else
		{
			cache = it->second;
			if (it != cache_glyphimage.begin())
			{
                swap(*it, cache_glyphimage.front());
			}
		}
	}
	cache_glyphimage_current = cache;
	style = i;
}

void BKE_Glyph::_drawGlyph(FT_Glyph glyph, QByteArray &buf, int &bufwidth, int &bufheight)
{
	FT_BBox bbox;
	FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_GRIDFIT, &bbox);
	this->left = bbox.xMin >> 6;
	this->top = font->ascent - (bbox.yMax >> 6);
	FT_Outline *outline = &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;

	bufwidth = FT_CEIL(bbox.xMax - bbox.xMin);
	bufheight = FT_CEIL(bbox.yMax - bbox.yMin);
	{
        buf.resize(bufwidth * bufheight);
        buf.fill(0, bufwidth * bufheight);

		FT_Raster_Params params;
		memset(&params, 0, sizeof(params));
		FT_Bitmap bmp;
        bmp.buffer = (unsigned char *)buf.data();
		bmp.width = bufwidth;
		bmp.rows = bufheight;
		bmp.pitch = bufwidth;
		bmp.pixel_mode = FT_PIXEL_MODE_GRAY;
		bmp.num_grays = 256;

		params.source = outline;
		params.target = &bmp;
		params.flags = FT_RASTER_FLAG_AA;
		FT_Outline_Translate(outline, -bbox.xMin, -bbox.yMin);
		FT_Outline_Render(BKE_FontCache::lib, outline, &params);
	}
}

BKE_Glyph::BKE_Glyph(bkushort ch, BKE_Font *f) :font(f)
//:width(glyph->bitmap.width + glyph->bitmap_left), height(f->height)
{
	//下一步优化：grayToRGBA和drawAt合并
	good = 0;
	FT_Error error = FT_Load_Char(f->face, ch, FT_LOAD_NO_BITMAP | FT_LOAD_FORCE_AUTOHINT);
	if (error) 
	{
		return;
	}
	FT_Glyph glyph;
	FT_Glyph glyph2;
	if (FT_Get_Glyph(f->face->glyph, &glyph))
	{
		return;
	}
	FT_Outline *outline = &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;

	int bearingY = f->face->glyph->metrics.horiBearingY >> 6;
	int bearingX = f->face->glyph->metrics.horiBearingX >> 6;

	this->advancex = FT_CEIL(f->face->glyph->advance.x);
	this->advancey = FT_CEIL(f->face->glyph->advance.y);

	if (f->is_italic()){
		FT_Matrix shear;
		shear.xx = 0x10000;
		shear.xy = 0x34FE; //~0.207
		shear.yx = 0;
		shear.yy = 0x10000;
		FT_Outline_Transform(outline, &shear);
	}
	if (f->is_bold())
		FT_Outline_Embolden(outline, FT_Pos((f->face->glyph->metrics.height >> 6) * 177 / 100)); //32 for every 18 of this->size
	//FT_Render_Glyph(this->face->glyph, FT_RENDER_MODE_NORMAL);
	//underline和strike交给BKE_Glyph处理
	if (f->style.enablestroke)
	{
		FT_Glyph_Copy(glyph, &glyph2);
	}
		
    QByteArray buf;
	int bufwidth;
	int bufheight;

	//FT_BBox bbox;
	//FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_GRIDFIT, &bbox);

	//this->left = bbox.xMin >> 6;
	//this->top = f->ascent - (bbox.yMax >> 6);

	_drawGlyph(glyph, buf, bufwidth, bufheight);

	width = bufwidth;
	height = bufheight;

	//if (f->is_italic())
	//{
	//	this->left -= (int)(this->height*0.207);
	//}

	if (f->style.enableshadow)
	{
		width += 2;
		height += 2;
	}

	int deltax = 0;
	int deltay = 0;

	if (f->style.enablestroke)
	{
		FT_Stroker stroker;
		FT_Stroker_New(BKE_FontCache::lib, &stroker);
		FT_Stroker_Set(stroker, (int)(1 * 64), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
		FT_Glyph_StrokeBorder(&glyph2, stroker, 0, 0);

        QByteArray strokebuf;
		int strokebufwidth;
		int strokebufrows;
		_drawGlyph(glyph2, strokebuf, strokebufwidth, strokebufrows);

        QByteArray stroke = grayToRGBA(strokebuf, strokebufwidth, strokebufrows, f->style.stroke);
        int newwidth = qMax(width, strokebufwidth);
        int newheight = qMax(height, strokebufrows);
        bitmap.resize(newwidth * newheight * 4);
        bitmap.fill(0, newwidth * newheight * 4);
		drawAt(bitmap, newwidth, newheight, stroke, strokebufwidth, strokebufrows, newwidth - strokebufwidth, newheight - strokebufrows);
		deltax = newwidth - width;
		deltay = newheight - height;
		this->left -= deltax;
		this->top -= deltay;
		width = newwidth;
		height = newheight;
	}
	else
	{
        bitmap.resize(width * height * 4);
        bitmap.fill(0, width * height * 4);
	}

	int d = this->height >> 4;
	int row;
	if (d<1)
		d = 1;
	if (f->is_strike())
	{
		row = (height - d) >> 1;
        for(auto i = row * width; i<row * width+width * d; i++)
            buf[i] = (char)0xFF;
	}
	if (f->is_underline())
	{
		row = height + f->underline_offset - d / 2;
		if (row>height - d)
			row = height - d;
        for(auto i = row * width; i<row * width+width * d; i++)
            buf[i] = (char)0xFF;
	}

	//handle shadow last
	if (f->style.enableshadow)
	{
        QByteArray shadow = grayToRGBA(buf, bufwidth, bufheight, f->style.shadow);
		drawAt(bitmap, width, height, shadow, bufwidth, bufheight, 1, 1);
	}

    QByteArray normal = grayToRGBA(buf, bufwidth, bufheight, f->style.color);
	drawAt(bitmap, width, height, normal, bufwidth, bufheight, deltax / 2, deltay / 2);

	FT_Done_Glyph(glyph);
	good = 1;
}

QMap<FontKey, BKE_Font*> BKE_FontCache::fontcache;
bklong BKE_FontCache::filecachesize = 0;
FT_Library BKE_FontCache::lib;
bool BKE_FontCache::isinited = false;
BKE_FontInfo BKE_FontCache::default_info;

static const char *font_list[] = {
#ifdef Q_OS_WIN
    "C:\\windows\\fonts\\msyh",
    "C:\\windows\\fonts\\simkai",
#endif
#ifdef Q_OS_LINUX
    "/usr/share/fonts/truetype/arphic/ukai",
    "/usr/share/fonts/chinese/TrueType/ukai",
    "/usr/share/fonts/ukai",
#endif
#ifdef Q_OS_MAC
    "/Library/Fonts/华文黑体.ttf",
    "/Library/Fonts/Arial Unicode.ttf",
#endif
	NULL
};

bool BKE_FontCache::setSystemFont(bklong size /* = 24 */, bkulong style /* = 0 */)
{
	BKE_FontInfo fi;
	fi.setFontSize(size);
	fi.setFontStyle(style);
	for (int i = 0; font_list[i]; i++)
	{
        QString filename = font_list[i];
		fi.setFontName(filename);
		if (!!fi)
		{
			default_info = fi;
			default_info.setYInterval(12);
			return true;
		}
	}
	return false;
}

void BKE_FontCache::setDefaultFont(const QString &name, bklong size, const BKE_FontColor &color)
{
	BKE_FontInfo fi = default_info;
	fi.setFontName(name);
	if (!!fi)
	{
		default_info = fi;
	}
	default_info.setFontSize(size);
	default_info.setFontColor(color);
	return;
}

bklong BKE_FontCache::drawChar(bkushort ch, QByteArray &data, bklong x, bklong y, int totalwidth, int totalheight, const BKE_FontInfo &info, bool forcedraw)
{
	auto f = info.getFont();
	if (!f)
	{
		return 0;
	}
	auto glyph = f->getGlyph(ch);
	if (!glyph)
		return 0;
    if (glyph->bitmap.size() == 0)
		return glyph->advancex;
	bkulong w = glyph->width;
	bkulong h = glyph->height;
	if (y + glyph->height > totalheight)
	{
		//do not check vertical
		//if (!forcedraw)
		//	return false;
		h = totalheight - y;
	}
	if (x + glyph->width > totalwidth)
	{
		if (!forcedraw)
			return -1;
		w = totalwidth - x;
	}
	if (w && h)
	{
		glyph->drawTo(data, totalwidth, totalheight, x, y, w, h);
	}
	//drawAt(data, totalwidth, totalheight, glyph->bitmap, w, h, x, y);
	return glyph->advancex;
}

//height最小为字体高，width最小为2，为了对空串的textsprite取高度也能有正确结果
void BKE_FontCache::getStringSize(const QString &text, bklong &width, bklong &height, const BKE_FontInfo &info/* = current_info*/)
{
	width = -info.getXInterval();
	BKE_Font *font = info.getFont();
	if (font == NULL)
	{
		width = 0;
		return;
	}
	height = font->height;
	int basepos = 0;
	for (int i = 0; i < (int)text.size(); i++)
	{
        BKE_Glyph *glyph = font->getGlyph(text[i].unicode());
		if (!glyph)
			continue;
		width = basepos + glyph->getActualWidth();
		basepos += glyph->advancex + info.getXInterval();
	}
	if (width < 2)
		width = 2;
}

QImage BKE_FontCache::getGlyphImage(const QString &text, const BKE_FontInfo &info)
{
	BKE_Font *font = info.getFont();
	if (!font)
	{
        return QImage();
	}
	int pos = -info.getXInterval();
	bklong width, height;
	getStringSize(text, width, height, info);
    QByteArray bitmap;
    bitmap.resize(width * height * 4);
    bitmap.fill(0, width * height * 4);
	for (int i = 0; i < (int)text.size(); i++)
	{
        bkushort ch = (bkushort)text[i].unicode();
		BKE_Glyph *glyph = font->getGlyph(ch);
		//字体不包含此字符，跳过
		if (!glyph)
			continue;
		//零宽度字符或空白字符，跳过
		//unsigned char *src = glyph->bitmap;
		//if (!src)
        if (glyph->bitmap.size() == 0)
		{
			pos += glyph->advancex + info.getXInterval();
			if (pos < 0)
				pos = 0;
			continue;
		}
		pos += info.getXInterval();
		glyph->drawTo(bitmap, width, height, pos, 0);
		//drawAt(bitmap, width, height, glyph->bitmap, glyph->width, glyph->height, pos, 0);
		pos += glyph->advancex;
		if (pos < 0)
			pos = 0;
	}
    return QImage((unsigned char *)bitmap.constData(), width, height, QImage::Format_RGBA8888);
}

QImage BKE_FontCache::getGlyphImageEx(const QString &text, const BKE_FontInfo &info/* = current_info*/, const QString &extraChar/* = L""*/, bklong w /*= -1*/, bklong h/* = -1*//*actually h is maxline*/)
{
	BKE_Font *font = info.getFont();
	if (font == NULL)
	{
        qWarning() << "BKE_FontCache: getGlyphImageEx: 字体为空。" << endl;
        return QImage();
	}
	bklong maxline = h;
	bklong ex = getStringSizeEx(text, w, h, extraChar, maxline, info);
    QByteArray bitmap;
    bitmap.resize(w * h * 4);
    bitmap.fill(0, w * h * 4);
	bklong end = text.size() - 1;
	//don't consider last '\n's and '\r's, so L"\n\n\n\n" still return height = 0
	while (end >= 0 && (text[end] == '\n' || text[end] == '\r'))
		end--;
	bklong curx = -info.getXInterval(), cury = 0;
	bklong curline = 1;
	bool islastline = (curline >= maxline);
	for (int i = 0; i <= end; i++)
	{
		if (text[i] == '\n' || text[i] == '\r')
		{
			//reline
			curx = -info.getXInterval();
			cury += font->lineskip + info.getYInterval();
			curline++;
			if (cury >= h)
				break;
			islastline = (curline >= maxline);
			continue;
		}
        BKE_Glyph *glyph = font->getGlyph(text[i].unicode());
		if (!glyph)
			continue;
        if (glyph->bitmap.size() == 0)
		{
			curx += glyph->advancex + info.getXInterval();
			if (curx < 0)
				curx = 0;
			continue;
		}
		if (curx + info.getXInterval() + glyph->getActualWidth() >(islastline ? w - ex : w))
		{
			//force return
			if (cury + font->lineskip + info.getYInterval() >= h)
				break;
			curx = -info.getXInterval();
			cury += font->lineskip + info.getYInterval();
			curline++;
			islastline = (curline >= maxline);
		}
		curx += info.getXInterval();
		glyph->drawTo(bitmap, w, h, curx, cury);
		//drawAt(bitmap, w, h, glyph->bitmap, glyph->width, glyph->height, curx, cury);
		curx += glyph->advancex;
	}
	if (ex > 0)
	{
		//draw extraChar
		for (int i = 0; i < extraChar.size(); i++)
		{
            BKE_Glyph *glyph = font->getGlyph(extraChar[i].unicode());
			if (!glyph)
				continue;
            if (glyph->bitmap.size() == 0)
			{
				curx += glyph->advancex + info.getXInterval();
				if (curx < 0)
					curx = 0;
				continue;
			}
			//pile glyph
			curx += info.getXInterval();
			glyph->drawTo(bitmap, w, h, curx, cury);
			//drawAt(bitmap, w, h, glyph->bitmap, glyph->width, glyph->height, curx, cury);
			curx += glyph->advancex;
		}
    }
    return QImage((unsigned char *)(bitmap.data()), w, h, QImage::Format_RGBA8888);
}

//height最小为字体高，width最小为2，为了对空串的textsprite取高度也能有正确结果
bklong BKE_FontCache::getStringSizeEx(const QString &text, bklong &width, bklong &height, const QString &extraChar, bklong &maxline, const BKE_FontInfo &info/* = current_info*/)
{
	bklong extracharwidth = 0, extracharheight = 0;
    if (!extraChar.isEmpty())
		getStringSize(extraChar, extracharwidth, extracharheight, info);
	bklong w = -info.getXInterval(), h = 0;
	bklong wmax = 0;
	bklong curline = 1;
	BKE_Font *font = info.getFont();
	if (font == NULL)
	{
		width = height = 0;
		return extracharwidth;
	}
	bklong curh = font->height;
	bklong end = text.size() - 1;
	//don't consider last '\n's and '\r's, so L"\n\n\n\n" still return height = 0
	while (end >= 0 && (text[end] == '\n' || text[end] == '\r'))
		end--;
	int base = 0;
	for (int i = 0; i <= end; i++)
	{
		if (text[i] == '\n' || text[i] == '\r')
		{
			//make a return
			if (wmax < w)
				wmax = w;
			if (height>0 && h + curh + font->lineskip + info.getYInterval() > height)
			{
				//break;
				height = h + curh;
				maxline = curline;
				if (width == -1 || width > wmax)
				{
					width = wmax;
				}
				return 0;
			}
			curline++;
			h += font->lineskip + info.getYInterval();
			//curh = 0;
			curh = font->height;
			base = 0;
			w = 0;
			continue;
		}
        BKE_Glyph *glyph = font->getGlyph(text[i].unicode());
		if (!glyph)
			continue;
		if (width > -1 && base + glyph->getActualWidth() > width)
		{
			//force return
			if (wmax < w)
				wmax = w;
			if (height>0 && h + curh + font->lineskip + info.getYInterval() > height)
			{
				//break;
				height = h + curh;
				maxline = curline;
				return extracharwidth;
			}
			curline++;
			h += font->lineskip + info.getYInterval();
			curh = font->height;
			w = 0;
			base = 0;
			i--;
			continue;
		}
		w = base + glyph->getActualWidth();
		base += glyph->advancex + info.getXInterval();
		//if (curh < glyph->height)
		//	curh = glyph->height;
		if (w < 0)
			w = 0;
	}
	if (wmax < w)
		wmax = w;
	if (width == -1 || width > wmax)
		width = wmax;
	height = h + curh;
	if (width < 2)
		width = 2;
	maxline = curline;
	return 0;
}

// 
// BKE_TextSprite* BKE_FontCache::getStringSprite(const QString &text, bklong xinterval, const ccColor4B &color)
// {
// 	if (current_font == NULL)
// 		return NULL;
// 	CCTexture2D *tex = getGlyphTexture(text,color, xinterval);
// 	if (!tex)
// 		return NULL;
// 	BKE_TextSprite *txt = BKE_TextSprite::create(tex, text, color);
// 	return txt;
// }

bool BKE_FontCache::initTTF()
{
	if (isinited)
		return true;
	isinited = !FT_Init_FreeType(&lib);
	if (isinited)
	{
		return setSystemFont();
	}
	return false;
}

//ulong Font_Read(FT_Stream s,ulong offset,uchar *buffer,ulong count)
//{
//	BKE_Stream *stream=(BKE_Stream *)s->descriptor.pointer;
//	ulong res=count;
//	if(offset+count>stream->size)
//		res=stream->size-offset;
//	memcpy(buffer, stream->data+offset, res);
//	return res;
//}

BKE_Font* BKE_FontCache::openFont(const QString &filename, bklong height)
{
    if(!QFile::exists(filename))
        return NULL;
    FontKey key(filename, height);
	auto it = fontcache.find(key);
	if (it != fontcache.end())
	{
        return it.value();
	}
	
    auto *f = BKE_FontCache::newFont(filename, height);
	if (!!f)
	{
        f->name = filename;
		return fontcache[key] = f;
	}
	else
		return NULL;
}

void BKE_FontCache::GCFont()
{
	//free font other than curfontname and curfont->name
	for (auto it = fontcache.begin(); it != fontcache.end(); it++)
	{
        if (!it.value()->isSingleReference())
			continue;
		//close this font
        it.value()->release();
		fontcache.erase(it);
	}
}

BKE_Font* BKE_FontCache::newFont(const QString &filename, bklong height)
{
	BKE_Font* font = new BKE_Font(lib, filename, height);
	if (!*font)
	{
		delete font;
		return NULL;
	}
	return font;
}

void BKE_FontCache::uninit()
{
	if (isinited)
	{
		for (auto it = fontcache.begin(); it != fontcache.end(); it++)
		{
            it.value()->release();
		}
		default_info.closeFont();
		FT_Done_FreeType(lib);
		lib = NULL;
		isinited = false;
	}
}
