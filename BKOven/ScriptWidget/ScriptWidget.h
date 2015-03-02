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

    static void open(const QString &projectPath);
    static void save();

    static bool addScript(const QString &type, const QString &description, const QBkeVariable &data);

private:
    static ScriptWidgetPrivate* _widget;

};

#endif // SCRIPTWIDGET_H
