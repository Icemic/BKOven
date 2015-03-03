#include "ScriptGenerator.h"
#include "../SozaiWidget/SozaiWidget.h"
#include "../StageWidget/StageWidget.h"
#include "../ScriptWidget/ScriptWidget.h"
#include <QDebug>

ScriptGenerator* ScriptGenerator::self = nullptr;

ScriptGenerator::ScriptGenerator()
{
}

ScriptGenerator::~ScriptGenerator()
{

}

void ScriptGenerator::init()
{
    self = new ScriptGenerator;
    connect(SozaiWidget::widget(),SIGNAL(fileAdd(QString,QString,QString,QBkeVariable)),self,SLOT(handleQuickAdd(QString,QString,QString,QBkeVariable)));
}

void ScriptGenerator::handleQuickAdd(const QString &type, const QString &name, const QString &filePath, const QBkeVariable &data)
{
    QBkeVariable value = data;
    value.insert("type",type);
    value.insert("name",name);
    ScriptWidget::addScript(type,value);
}
