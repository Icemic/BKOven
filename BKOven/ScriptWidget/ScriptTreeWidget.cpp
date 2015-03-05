#include "ScriptTreeWidget.h"
#include "../shared/vshack.h"
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QUuid>
#include <QDebug>

ScriptTreeWidget::ScriptTreeWidget(QWidget *parent):
    QTreeWidget(parent)
{
    setHeaderHidden(true);

    rMenu = new QMenu();
    QAction *blank_add = new QAction("添加幕",rMenu);
    QAction *parent_add = new QAction("添加场景",rMenu);
    QAction *parent_delete = new QAction("删除",rMenu);
    QAction *child_show = new QAction("查看剧本",rMenu);
    QAction *child_delete = new QAction("删除",rMenu);

    blankMenuActions.append(blank_add);
    parentMenuActions.append(parent_add);
    parentMenuActions.append(parent_delete);
    childMenuActions.append(child_show);
    childMenuActions.append(child_delete);

    //添加幕
    connect(blank_add,&QAction::triggered,[=](bool){
        QString name;
        for(;;){
            bool isOk;
            name = QInputDialog::getText(this, tr("添加幕"),
                                               tr("请输入"),
                                               QLineEdit::Normal,
                                               tr("新的一幕"),
                                               &isOk);
            if(!isOk)
                return; //若点击取消
            if(this->findItems(name,Qt::MatchExactly).count()==0)
                break;
            else
                QMessageBox::warning(this,"添加幕","已存在名为 "+name+" 的幕，请使用其他名称",QMessageBox::Ok,QMessageBox::NoButton);
        }

        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setData(0,Qt::UserRole,0);
        item->setText(0,name);
        this->addTopLevelItem(item);
    });

    //添加场景
    connect(parent_add,&QAction::triggered,[=](bool){
        QTreeWidgetItem* parent = this->currentItem();
        QString name;
        for(;;){
            bool isOk;
            name = QInputDialog::getText(this, tr("添加场景"),
                                               tr("请输入"),
                                               QLineEdit::Normal,
                                               tr("新场景"),
                                               &isOk);
            if(!isOk)
                return; //若点击取消

            //查重
            bool findSame = false;
            for(int i=0;i<parent->childCount();i++){
                QTreeWidgetItem* child = parent->child(i);
                if(child->text(0)==name){
                    findSame = true;
                    break;
                }
            }

            if(findSame)
                QMessageBox::warning(this,"添加场景","已存在名为 "+name+" 的场景，请使用其他名称",QMessageBox::Ok,QMessageBox::NoButton);
            else
                break;
        }

        QTreeWidgetItem* item = new QTreeWidgetItem();
        item->setData(0,Qt::UserRole  ,1);
        item->setData(0,Qt::UserRole+1,QUuid::createUuid().toString());
        qDebug() << item->data(0,Qt::UserRole+1).toString();
        item->setText(0,name);
        parent->addChild(item);
    });

    connect(parent_delete,&QAction::triggered,[=](bool){
        int result = QMessageBox::warning(this,"删除幕","删除幕的同时也会删除其中的场景，且删除不可恢复。\n确定删除吗？",QMessageBox::Yes,QMessageBox::No);
        if(result==QMessageBox::Yes){
            QTreeWidgetItem* item = this->currentItem();
            this->takeTopLevelItem(this->indexOfTopLevelItem(this->currentItem()));
            for(int i=0;i<item->childCount();i++){
                delete item->child(i);
            }
            delete item;
        }
    });

    connect(child_delete,&QAction::triggered,[=](bool){
        int result = QMessageBox::warning(this,"删除场景","场景删除不可恢复。\n确定删除吗？",QMessageBox::Yes,QMessageBox::No);
        if(result==QMessageBox::Yes){
            QTreeWidgetItem* item = this->currentItem();
            item->parent()->removeChild(item);
            delete item;
        }
    });

    connect(child_show,&QAction::triggered,[=](bool){
        emit requestShowScripts(this->currentItem()->data(0,Qt::UserRole+1).toString());
    });

    //双击
    connect(this,&ScriptTreeWidget::itemDoubleClicked,[=](QTreeWidgetItem* current, int){
        if(current->data(0,Qt::UserRole)==1){
            qDebug() << current->data(0,Qt::UserRole+1).toString() << endl;
            emit requestShowScripts(current->data(0,Qt::UserRole+1).toString());
        }
    });
}

ScriptTreeWidget::~ScriptTreeWidget()
{
    delete rMenu;
}

void ScriptTreeWidget::contextMenuEvent(QContextMenuEvent *)
{
    QTreeWidgetItem* item = this->itemAt(mapFromGlobal(QCursor::pos()));
    if(item == nullptr)
        rMenu->exec(blankMenuActions,QCursor::pos());
    else if(item->data(0,Qt::UserRole)==0)
        rMenu->exec(parentMenuActions,QCursor::pos());
    else
        rMenu->exec(childMenuActions,QCursor::pos());
}

QBkeVariable ScriptTreeWidget::scriptData()
{
    QBkeVariable dic = QBkeVariable::dic();
    for(int i=0;i<this->topLevelItemCount();i++)
    {
        QTreeWidgetItem* topItem = this->topLevelItem(i);
        QBkeVariable subDic = QBkeVariable::dic();
        for(int j=0;j<topItem->childCount();j++)
        {
            subDic.insert(topItem->child(j)->text(0),topItem->child(j)->data(0,Qt::UserRole+1).toString());
        }
        dic.insert(topItem->text(0),subDic);
    }
    return dic;
}

void ScriptTreeWidget::setScriptData(const QBkeDictionary &dic)
{
    this->clear();
    QBkeDictionary::const_iterator ite;
    for(ite=dic.cbegin();ite!=dic.cend();ite++)
    {
        QTreeWidgetItem* topItem = new QTreeWidgetItem();
        topItem->setData(0,Qt::UserRole,0);
        topItem->setText(0,ite.key());

        QBkeDictionary subDic = ite.value().toBkeDic();
        QBkeDictionary::const_iterator subIte;
        for(subIte=subDic.cbegin();subIte!=subDic.cend();subIte++)
        {
            QTreeWidgetItem* item = new QTreeWidgetItem();
            item->setData(0,Qt::UserRole  ,1);
            item->setData(0,Qt::UserRole+1,subIte.value().toString());
            item->setText(0,subIte.key());
            topItem->addChild(item);
        }
        this->addTopLevelItem(topItem);
    }
}
