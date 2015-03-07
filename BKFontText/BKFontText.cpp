#include "BKFontText.h"
#include "../shared/vshack.h"

BKFontText::BKFontText()
{
    BKE_FontCache::initTTF();
}

BKFontText::~BKFontText()
{
    BKE_FontCache::uninit();
}

BKFontText *BKFontText::getInstance()
{
    static BKFontText a;
    return &a;
}

QImage BKFontText::render(const QString &text)
{
    BKE_FontInfo fInfo;
    fInfo.setFontName("D:/SourceHanSansCN-Normal.otf");
    fInfo.setFontColor(BKE_FontColor(QColor("black")));
    fInfo.setFontStyle(bkeStyleNormal);
    fInfo.setFontSize(24);


    return BKE_FontCache::getGlyphImageEx(text,fInfo);
}
