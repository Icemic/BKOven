#include "ScriptWidget.h"
#include <QDebug>

ScriptWidgetPrivate* ScriptWidget::_widget = nullptr;
ScriptWidget::ScriptWidget()
{

}

ScriptWidget::~ScriptWidget()
{

}

void ScriptWidget::test()
{
    qDebug() << "ScriptWidget: ok!" << endl;
}

void ScriptWidget::init()
{
    _widget = new ScriptWidgetPrivate();
}

ScriptWidgetPrivate* ScriptWidget::widget()
{
    return _widget;
}

