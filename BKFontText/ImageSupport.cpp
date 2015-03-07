#include "ImageSupport.h"
#include "Font.h"

QByteArray grayToRGBA(const QByteArray &gray, bkulong width, bkulong height, const BKE_FontColor &color)
{
    QByteArray rgba;
    rgba.resize(width * height * 4);
	rgba.fill(0, width * height * 4);
    bkulong *dst = (bkulong *)rgba.data();
    bkuchar *src = (bkuchar *)gray.data();
	bkulong b, c;
	if (color.isSingleColor())
	{
        b = (color.from.rgba() & 0x00FFFFFF);
		while (height--)
		{
			int ww = (width + 3) / 4;
			switch (width & 3)
			{
			case 0:
				do
				{
				c = b | (*src++ << 24);
				*dst++ = c;
			case 3:
				c = b | (*src++ << 24);
				*dst++ = c;
			case 2:
				c = b | (*src++ << 24);
				*dst++ = c;
			case 1:
				c = b | (*src++ << 24);
				*dst++ = c;
				} while (--ww > 0);
			}
		}
	}
	else
	{
		for (int i = 0; i < height; i++)
		{
            QColor t = QColor((color.to.red() - color.from.red()) * i / height + color.from.red(),
                (color.to.green() - color.from.green()) * i / height + color.from.green(),
                (color.to.blue() - color.from.blue()) * i / height + color.from.blue(),
                255
				);
            b = t.rgba();
			int ww = (width + 3) / 4;
			switch (width & 3)
			{
			case 0:
				do
				{
				c = b | (*src++ << 24);
				*dst++ = c;
			case 3:
				c = b | (*src++ << 24);
				*dst++ = c;
			case 2:
				c = b | (*src++ << 24);
				*dst++ = c;
			case 1:
				c = b | (*src++ << 24);
				*dst++ = c;
				} while (--ww > 0);
			}
		}
	}
	return rgba;
}

#define DRAW_ONCE \
		{				\
			bklong dalpha=dst[base + 3];								\
			bklong srcalpha=src[base2 + 3];							\
			bklong s=*(bklong*)&src[base2];							\
			bklong &d=*(bklong*)&dst[base];							\
			if (dalpha == 0 || srcalpha == 255)							\
				d = s;													\
			else if (srcalpha != 0)									\
			{															\
				bklong s1 = s & 0xff00ff;								\
				bklong d1 = d & 0xff00ff;								\
				d1 = (d1 + ((s1 - d1) * srcalpha >> 8)) & 0xff00ff;		\
				s &= 0xff00;											\
				d &= 0xff00;											\
				d = (d + ((s - d) * srcalpha >> 8)) & 0xff00;			\
				dalpha = 0xFF ^ (((dalpha ^ 0xFF)*(srcalpha ^ 0xFF)) >> 8);		\
				d |= d1 | (dalpha<<24);									\
			}															\
		}

//ABGR => ABGR
void drawAt(QByteArray &dst_, bkulong dstWidth, bkulong dstHeight, const QByteArray &src_, bkulong srcWidth, bkulong srcHeight, bklong x, bklong y)
{
	if ((bklong)dstWidth < x || (bklong)dstHeight < y)
		return;
    char *dst = dst_.data();
    const char *src = src_.data();
	int width, height;
	if (x >= 0)
        width = (int)(qMin(srcWidth, dstWidth - x));
	else
        width = (int)qMin(srcWidth + x, dstWidth);
	if (y >= 0)
        height = (int)(qMin(srcHeight, dstHeight - y));
	else
        height = (int)qMin(srcHeight + y, dstHeight);
	int srcskip = 4 * (srcWidth - width);
	int dstskip = 4 * (dstWidth - width);
	int base, base2;
	base = y>0?y*dstWidth:0;
	base += x>0 ? x : 0;
	base *= 4;
	base2 = y < 0 ? -y*srcWidth : 0;
	base2 -= x < 0 ? x : 0;
	base2 *= 4;
	if (!width || !height)
		return;
	while (height--)
	{
		int ww = (width + 3) / 4;
		switch (width & 3)
		{
		case 0:
			do
			{
			DRAW_ONCE;
			base += 4; base2 += 4;
		case 3:
			DRAW_ONCE;
			base += 4; base2 += 4;
		case 2:
			DRAW_ONCE;
			base += 4; base2 += 4;
		case 1:
			DRAW_ONCE;
			base += 4; base2 += 4;
			} while (--ww > 0);
		}
		base += dstskip;
		base2 += srcskip;
	}
}
