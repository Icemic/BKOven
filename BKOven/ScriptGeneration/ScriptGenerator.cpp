#include "ScriptGenerator.h"

ScriptGenerator::ScriptGenerator()
{
    connect(SozaiWidget::widget(),SIGNAL(fileAdd(QString,QString,QString,bool,int,int,int)),this,SLOT(handleQuickAdd(QString,QString,QString,bool,int,int,int)));
}

ScriptGenerator::~ScriptGenerator()
{

}

void ScriptGenerator::handleQuickAdd(const QString &type, const QString &name, const QString &filePath, bool fade, int from, int to, int msec)
{

}
