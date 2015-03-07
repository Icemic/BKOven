#pragma once

#include "Common.h"
#include <QtCore>

struct BKE_FontColor;
QByteArray grayToRGBA(const QByteArray &gray, bkulong width, bkulong height, const BKE_FontColor &color);

void drawAt(QByteArray &dst, bkulong dstWidth, bkulong dstHeight, const QByteArray &src, bkulong srcWidth, bkulong srcHeight, bklong x, bklong y);
