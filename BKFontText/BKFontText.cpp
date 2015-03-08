#include "BKFontText.h"
#include "../shared/vshack.h"
#include <QPixmap>
#include <QDebug>

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

QPixmap BKFontText::render(const QString &text, const QString &infoName, int w, int h)
{
    if(fontInfoMap.contains(infoName))
    {
        BKE_FontInfo &fInfo = fontInfoMap[infoName];
        return QPixmap::fromImage(BKE_FontCache::getGlyphImageEx(text,fInfo,"...",w,h));
    }
    else
    {
        qWarning() << "BKFontText: 找不到对应的FontInfo" << endl;
        return QPixmap();
    }
}

void BKFontText::setFontInfo(const QString &infoName, const QString &name, const QColor &color, const bklong &style, int size)
{
    BKE_FontInfo fInfo;
    if(fontInfoMap.contains(infoName))
    {
        fInfo = fontInfoMap[infoName];
    }
    if(fInfo.getFontName()!=name)
    {
        fInfo.closeFont();
        fInfo.setFontName(name);
    }
    fInfo.setFontColor(BKE_FontColor(color));
    fInfo.setFontStyle(style);
    fInfo.setFontSize(size);
    fontInfoMap.insert(infoName,fInfo);
}

void BKFontText::removeFontInfo(const QString &infoName)
{
    if(fontInfoMap.contains(infoName))
    {
        BKE_FontInfo &fInfo = fontInfoMap[infoName];
        fInfo.closeFont();
        fontInfoMap.remove(infoName);
    }
    else
    {
        qWarning() << "BKFontText: 找不到对应的FontInfo" << endl;
    }
}

void BKFontText::setBold(const QString &infoName, bool state)
{
    if(fontInfoMap.contains(infoName))
    {
        BKE_FontInfo &fInfo = fontInfoMap[infoName];
        if(state)
            fInfo.setFontStyle(fInfo.getFontStyle()|bkeStyleBold);
        else
            fInfo.setFontStyle(fInfo.getFontStyle()&~bkeStyleBold);
    }
    else
    {
        qWarning() << "BKFontText: 找不到对应的FontInfo" << endl;
    }
}

void BKFontText::setItalic(const QString &infoName, bool state)
{
    if(fontInfoMap.contains(infoName))
    {
        BKE_FontInfo &fInfo = fontInfoMap[infoName];
        if(state)
            fInfo.setFontStyle(fInfo.getFontStyle()|bkeStyleItalic);
        else
            fInfo.setFontStyle(fInfo.getFontStyle()&~bkeStyleItalic);
    }
    else
    {
        qWarning() << "BKFontText: 找不到对应的FontInfo" << endl;
    }
}

void BKFontText::setUnderlined(const QString &infoName, bool state)
{
    if(fontInfoMap.contains(infoName))
    {
        BKE_FontInfo &fInfo = fontInfoMap[infoName];
        if(state)
            fInfo.setFontStyle(fInfo.getFontStyle()|bkeStyleUnderline);
        else
            fInfo.setFontStyle(fInfo.getFontStyle()&~bkeStyleUnderline);
    }
    else
    {
        qWarning() << "BKFontText: 找不到对应的FontInfo" << endl;
    }
}

void BKFontText::setStrike(const QString &infoName, bool state)
{
    if(fontInfoMap.contains(infoName))
    {
        BKE_FontInfo &fInfo = fontInfoMap[infoName];
        if(state)
            fInfo.setFontStyle(fInfo.getFontStyle()|bkeStyleStrikethrough);
        else
            fInfo.setFontStyle(fInfo.getFontStyle()&~bkeStyleStrikethrough);
    }
    else
    {
        qWarning() << "BKFontText: 找不到对应的FontInfo" << endl;
    }
}

void BKFontText::setShadow(const QString &infoName, bool state, const QColor &color)
{
    if(fontInfoMap.contains(infoName))
    {
        BKE_FontInfo &fInfo = fontInfoMap[infoName];
        fInfo.setShadowEnabled(state);
        if(color.isValid())
            fInfo.setShadow(color);
    }
    else
    {
        qWarning() << "BKFontText: 找不到对应的FontInfo" << endl;
    }
}

void BKFontText::setStroke(const QString &infoName, bool state, const QColor &color)
{
    if(fontInfoMap.contains(infoName))
    {
        BKE_FontInfo &fInfo = fontInfoMap[infoName];
        fInfo.setStrokeEnabled(state);
        if(color.isValid())
            fInfo.setStroke(color);
    }
    else
    {
        qWarning() << "BKFontText: 找不到对应的FontInfo" << endl;
    }
}
