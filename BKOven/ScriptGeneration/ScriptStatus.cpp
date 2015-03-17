#include "ScriptStatus.h"

ScriptStatus* ScriptStatus::_self = nullptr;
ScriptStatus::ScriptStatus()
{
    doc = QBkeVariable::array(0);
}

ScriptStatus::~ScriptStatus()
{

}

void ScriptStatus::init()
{
    _self = new ScriptStatus;
}

ScriptStatus* ScriptStatus::self()
{
    return _self;
}

void ScriptStatus::open(const QString &projectPath, const QString &uuid)
{
    this->projectPath = projectPath;
    this->uuid = uuid;
    QFile f(projectPath+"/config/ScriptStatus/"+uuid+".bkpsr");
    f.open(QIODevice::ReadOnly);
    doc.loadFromBinary(f.readAll());
    if(doc.isNull()) doc = doc.dic();
    f.close();
}

void ScriptStatus::save()
{
    QDir dir(projectPath+"/config");
    dir.mkpath("ScriptStatus");

    QFile f(projectPath+"/config/ScriptStatus/"+uuid+".bkpsr");
    f.open(QIODevice::WriteOnly | QIODevice::Truncate);
    f.write(doc.saveToBinary(true));
    f.close();
}

QBkeVariable ScriptStatus::currentStatus(int row)
{
    return doc[row];
}

void ScriptStatus::_analysis(QBkeVariable &item, const QBkeVariable &data)
{
    QString type = data.value("type").toString();           //类型
    if(type=="background")
    {
        item["background"] = data;
        return;
    }
    else if(type=="character")
    {
        if(data.value("freepos").toBool())
            item["character"][data.value("name").toString()] = data;
        else{
            int pos = data.value("pos").toInt();
            if(characterFixedList.contains(pos))
            {
                item["character"].remove(characterFixedList.value(pos));
            }
            item["character"][data.value("name").toString()] = data;
            characterFixedList.insert(pos,data.value("name").toString());
        }
        return;
    }
    else if(type=="item")
    {
        item["item"][data.value("name").toString()] = data;
        return;
    }
    else if(type=="bgm")
    {
        item["bgm"] = data;
        return;
    }
    else if(type=="se")
    {
        //这个……
        item["se"][data.value("name").toString()] = data;
        return;
    }
    else
        qWarning() << "ScriptStatus: 未知的类型 "+type << endl;
}

void ScriptStatus::analysis(ScriptListWidget *widget)
{
    QBkeVariable item = QBkeVariable::dic();
    item["background"] = QBkeVariable::dic();
    item["character"] = QBkeVariable::dic();
    item["item"] = QBkeVariable::dic();
    item["sound"] = QBkeVariable::dic();
    //背景 立绘 背景音乐
    int I = widget->count();
    qDebug() << I;
    for(int i=0;i<I;i++)
    {
        QBkeVariable data = widget->script(i)->scriptData();    //当前项i的剧本数据
        _analysis(item,data);
        doc[i] = item.clone();
        qDebug() << doc.saveToString();
    }
}

void ScriptStatus::analysisOne(ScriptListWidget *widget, int row)
{
    if(doc.getCount()<=row/2)
    {
        analysis(widget);
        return;
    }
    else if(doc.getCount()<=row)
    {
        if(doc[row-1].isVoid())
            analysisOne(widget,row-1);
        QBkeVariable data = widget->script(row)->scriptData();
        QBkeVariable item = doc[row-1].value().clone();
        _analysis(item,data);
        doc[row] = item;
    }
    else
        return;
}

void ScriptStatus::dropFrom(int row)
{
    int i,I=doc.getCount();
    for(i=row;i<I;i++)
    {
        doc.remove(i);
    }
}

void ScriptStatus::dropAfter(int row)
{
    dropFrom(row+1);
}
