#ifndef BKFONTTEXT_H
#define BKFONTTEXT_H
#include "bkfonttext_global.h"
#include "Font.h"


class BKFONTTEXTSHARED_EXPORT BKFontText
{
    BKFontText();
    ~BKFontText();
public:
    static BKFontText* getInstance();

    QPixmap render(const QString &text, const QString &infoName, int w=-1, int h=-1);
    void setFontInfo(const QString &infoName, const QString &name, const QColor &color=QColor("white"), const bklong &style=bkeStyleNormal, int size=24);
    void setBold(const QString &infoName, bool state);
    void setItalic(const QString &infoName, bool state);
    void setUnderlined(const QString &infoName, bool state);
    void setStrike(const QString &infoName, bool state);

    void removeFontInfo(const QString &infoName);

    void setShadow(const QString &infoName, bool state, const QColor &color=QColor());
    void setStroke(const QString &infoName, bool state, const QColor &color=QColor());

private:
    QMap<QString,BKE_FontInfo> fontInfoMap;


    void setStyle();
    void printOne(const QString &ch);


};

#endif // BKFONTTEXT_H
