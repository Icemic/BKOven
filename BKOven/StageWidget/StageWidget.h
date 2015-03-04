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

    static void setStageStatus(const QBkeVariable &data);

private:
    static StageWidgetPrivate* _widget;

};

#endif // STAGEWIDGET_H
