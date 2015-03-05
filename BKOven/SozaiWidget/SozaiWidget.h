#ifndef SOZAIWIDGET_H
#define SOZAIWIDGET_H

#include <QtCore>
#include "SozaiWidgetPrivate.h"

class SozaiWidget
{

public:
    explicit SozaiWidget();

    static void test();
    static void init();
    static SozaiWidgetPrivate* widget();

    static void open(const QString &projectPath);

private:
    static SozaiWidgetPrivate* _widget;

};

#endif // SOZAIWIDGET_H
