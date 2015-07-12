#ifndef PREVIEWAREA_H
#define PREVIEWAREA_H
#include "../PreviewWidget/PreviewWidget.h"
#include "../ParserHelper/ParserHelper.h"

class PreviewArea
{
public:
    explicit PreviewArea();
    ~PreviewArea();

    static void test();
    static void init();
    static void open(const QString &projectPath, int w, int h);
    static void update(const QBkeVariable &doc);
    static PreviewWidget* widget();

private:
    static QString projectPath;
    static PreviewWidget* _widget;
};

#endif // PREVIEWAREA_H
