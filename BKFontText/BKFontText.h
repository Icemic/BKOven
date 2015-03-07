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

    QImage render(const QString &text);

private:
    void setStyle();
    void printOne(const QString &ch);


};

#endif // BKFONTTEXT_H
