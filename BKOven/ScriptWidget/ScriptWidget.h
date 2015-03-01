#ifndef SCRIPTWIDGET_H
#define SCRIPTWIDGET_H
#include "ScriptWidgetPrivate.h"

class ScriptWidget
{
public:
    explicit ScriptWidget();
    ~ScriptWidget();

    static void test();
    static void init();
    static ScriptWidgetPrivate* widget();

private:
    static ScriptWidgetPrivate* _widget;

};

#endif // SCRIPTWIDGET_H
