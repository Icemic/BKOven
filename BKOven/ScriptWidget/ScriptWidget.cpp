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

void ScriptWidget::open(const QString &projectPath)
{
    _widget->open(projectPath);
}

void ScriptWidget::save()
{
    _widget->save();
}

bool ScriptWidget::addScript(const QString &type, const QBkeVariable &data)
{
    return _widget->addScript(type,data);
}
