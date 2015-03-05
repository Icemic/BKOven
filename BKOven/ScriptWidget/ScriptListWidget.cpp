#include "ScriptListWidget.h"
#include "../ScriptGeneration/DescriptionGenerator.h"
#include <QMessageBox>

ScriptListWidget::ScriptListWidget(QWidget *parent):
    QListWidget(parent)
{
    rMenu = new QMenu();
    QAction* item_delete = new QAction(tr("删除选中"),rMenu);
    QAction* item_insertpoint = new QAction(tr("设为插入点"),rMenu);
    item_insertpoint->setToolTip(tr("设为插入点后，新的剧本项目将插入到此项之后，而非剧本末端。"));
    itemMenuActions.append(item_delete);
    itemMenuActions.append(item_insertpoint);

    connect(item_delete,&QAction::triggered,[=](){
        int result = QMessageBox::question(this,tr("删除"),tr("删除后不可还原，确认删除吗？"),QMessageBox::Yes,QMessageBox::No);
        if(result==QMessageBox::Yes){
            QList<QListWidgetItem*> selectedItems = this->selectedItems();
            for(int i=0;i<selectedItems.length();i++)
            {
                QListWidgetItem* item = selectedItems.at(i);
                this->takeItem(this->row(item));
                if(item==insertPoint)
                    insertPoint = nullptr;
                delete item;
            }
        }
    });

    insertPoint = nullptr;
    connect(item_insertpoint,&QAction::triggered,[=](){
        if(insertPoint!=nullptr)
            insertPoint->setBackgroundColor(QColor("transparent"));
        insertPoint = this->currentItem();
        insertPoint->setBackgroundColor(QColor("#9fc888"));
    });


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
    delete rMenu;
}

void ScriptListWidget::contextMenuEvent(QContextMenuEvent *)
{
    QListWidgetItem* item = this->itemAt(mapFromGlobal(QCursor::pos()));
    if(item == nullptr)
        return;
    else
        rMenu->exec(itemMenuActions,QCursor::pos());
}

void ScriptListWidget::addScript(const QString &type, const QBkeVariable &data)
{
    ScriptListWidgetItem* item = new ScriptListWidgetItem();
    item->setType(type);
    item->setDescription(DescriptionGenerator::description(type,data));
    item->setScriptData(data);
    if(insertPoint==nullptr)
        this->addItem(item);
    else
        this->insertItem(row(insertPoint)+1,item);
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
        this->addScript(itemVar.value("type").toString(),itemVar.value("data").value());
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
