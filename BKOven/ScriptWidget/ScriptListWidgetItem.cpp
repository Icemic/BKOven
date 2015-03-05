#include "ScriptListWidgetItem.h"

ScriptListWidgetItem::ScriptListWidgetItem():
    QListWidgetItem()
{

}

ScriptListWidgetItem::~ScriptListWidgetItem()
{

}

void ScriptListWidgetItem::setType(const QString &type)
{
    _type = type;
}

void ScriptListWidgetItem::setScriptData(const QBkeVariable &data)
{
    _scriptData = data;
}

void ScriptListWidgetItem::setDescription(const QString &text)
{
    _description = text;
    setText(text);
}

QString ScriptListWidgetItem::type()
{
    return _type;
}

QString ScriptListWidgetItem::description()
{
    return _description;
}

QBkeVariable ScriptListWidgetItem::scriptData()
{
    return _scriptData;
}

QBkeVariableRef ScriptListWidgetItem::scriptDataRef()
{
    return _scriptData.ref();
}
