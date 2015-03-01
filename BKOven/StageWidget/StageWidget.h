#ifndef STAGEWIDGET_H
#define STAGEWIDGET_H
#include "StageWidgetPrivate.h"

class StageWidget
{
public:
    explicit StageWidget();
    ~StageWidget();

    static void test();
    static void init();
    static StageWidgetPrivate *widget();

private:
    static StageWidgetPrivate* _widget;

};

#endif // STAGEWIDGET_H
