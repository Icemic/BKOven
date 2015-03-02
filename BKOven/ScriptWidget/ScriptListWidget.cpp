#include "ScriptListWidget.h"

ScriptListWidget::ScriptListWidget(QWidget *parent):
    QListWidget(parent)
{
    connect(this,&ScriptListWidget::itemClicked,[=](QListWidgetItem* item){
        ScriptListWidgetItem* sItem = static_cast<ScriptListWidgetItem*>(item);
        emit scriptClicked(sItem->type(),sItem->description(),sItem->scriptDataRef());
    });

    connect(this,&ScriptListWidget::itemDoubleClicked,[=](QListWidgetItem* item){
        ScriptListWidgetItem* sItem = static_cast<ScriptListWidgetItem*>(item);
        emit scriptDoubleClicked(sItem->type(),sItem->description(),sItem->scriptDataRef());
    });

}

ScriptListWidget::~ScriptListWidget()
{

}

void ScriptListWidget::addScript(const QString &type, const QString &description, const QBkeVariable &data)
{
    ScriptListWidgetItem* item = new ScriptListWidgetItem();
    item->setType(type);
    item->setDescription(description);
    item->setScriptData(data);
    this->addItem(item);
}

ScriptListWidgetItem* ScriptListWidget::script(int row)
{
    return static_cast<ScriptListWidgetItem*>(item(row));
}

QBkeVariable ScriptListWidget::scriptData()
{
    QBkeVariable arr = QBkeVariable::array(0);
    for(int i=0;i<this->count();i++)
    {
        QBkeVariable itemVar = QBkeVariable::dic();
        ScriptListWidgetItem* item = script(i);
        itemVar.insert("type",item->type());
        itemVar.insert("description",item->description());
        itemVar.insert("data",item->scriptData());
        arr.append(itemVar);
    }
    return arr;
}

void ScriptListWidget::setScriptData(const QBkeArray &arr)
{
    this->clear();
    for(int i=0;i<arr.count();i++)
    {
        QBkeVariable itemVar = arr.at(i);
        this->addScript(itemVar.value("type").toString(),itemVar.value("description").toString(),itemVar.value("data").value());
    }
}

void ScriptListWidget::setUuid(const QString &uuid)
{
    _uuid = uuid;
}

QString ScriptListWidget::uuid()
{
    return _uuid;
}
