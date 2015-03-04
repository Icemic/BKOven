#include "ScriptListWidget.h"
#include "../ScriptGeneration/DescriptionGenerator.h"
#include <QMessageBox>

ScriptListWidget::ScriptListWidget(QWidget *parent):
    QListWidget(parent)
{
    rMenu = new QMenu();
    QAction* item_delete = new QAction(tr("删除选中"),rMenu);
    itemMenuActions.append(item_delete);

    connect(item_delete,&QAction::triggered,[=](){
        int result = QMessageBox::question(this,tr("删除"),tr("删除后不可还原，确认删除吗？"),QMessageBox::Yes,QMessageBox::No);
        if(result==QMessageBox::Yes){
            QList<QListWidgetItem*> selectedItems = this->selectedItems();
            int start = this->row(selectedItems.at(0));
            int count = selectedItems.count();
            for(int i=0;i<selectedItems.length();i++)
            {
                QListWidgetItem* item = selectedItems.at(i);
                this->takeItem(this->row(item));
                delete item;
            }
            emit scriptDeleted(start,count);
        }
    });

    //选中项改变底色
    connect(this,&ScriptListWidget::currentItemChanged,[=](QListWidgetItem* current, QListWidgetItem* previous){
        if(previous!=nullptr)
            previous->setBackgroundColor(QColor("transparent"));
        if(current!=nullptr)
        current->setBackgroundColor(QColor("#9fc888"));
    });


    connect(this,&ScriptListWidget::itemClicked,[=](QListWidgetItem* item){
        int row = this->row(item);
        ScriptListWidgetItem* sItem = static_cast<ScriptListWidgetItem*>(item);
        emit scriptClicked(sItem->type(),sItem->scriptDataRef(),row);
    });

    connect(this,&ScriptListWidget::itemDoubleClicked,[=](QListWidgetItem* item){
        int row = this->row(item);
        ScriptListWidgetItem* sItem = static_cast<ScriptListWidgetItem*>(item);
        emit scriptDoubleClicked(sItem->type(),sItem->scriptDataRef(),row);
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

//外部调用接口，触发信号
void ScriptListWidget::addScript(const QString &type, const QBkeVariable &data)
{
    _addScript(type,data);
    emit scriptAdded(currentRow());
}

//内部调用接口，不触发脚本添加信号
void ScriptListWidget::_addScript(const QString &type, const QBkeVariable &data)
{
    ScriptListWidgetItem* item = new ScriptListWidgetItem();
    item->setType(type);
    item->setDescription(DescriptionGenerator::description(type,data));
    item->setScriptData(data);
    this->insertItem(currentRow()+1,item);
    setCurrentRow(currentRow()+1);
}

ScriptListWidgetItem* ScriptListWidget::script(int row)
{
    return static_cast<ScriptListWidgetItem*>(item(row));
}

void ScriptListWidget::freshDescription(int row)
{
    ScriptListWidgetItem* item = script(row);
    item->setDescription(DescriptionGenerator::description(item->type(),item->scriptData()));
}

//取得全部脚本数据（数组）
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

//置全部脚本数据（数组）
void ScriptListWidget::setScriptData(const QBkeArray &arr)
{
    this->clear();
    for(int i=0;i<arr.count();i++)
    {
        QBkeVariable itemVar = arr.at(i);
        this->_addScript(itemVar.value("type").toString(),itemVar.value("data").value());
    }
}

//标记当前显示的数据的UUID
void ScriptListWidget::setUuid(const QString &uuid)
{
    _uuid = uuid;
}

QString ScriptListWidget::uuid()
{
    return _uuid;
}
