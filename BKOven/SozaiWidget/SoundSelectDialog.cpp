#include "SoundSelectDialog.h"
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QListWidgetItem>
#include <QMenu>
#include "BKAudio.h"

SoundSelectDialog::SoundSelectDialog(QWidget *parent) :
    QListWidget(parent)
{
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    this->setMouseTracking(true);
    this->setUniformItemSizes(true);


    //设置右键菜单
    rMenu = new QMenu(this);
    QAction* action_quickadd = new QAction("一键登场",rMenu);
    QAction* action_add = new QAction("登场",rMenu);
    QAction* action_open = new QAction("打开",rMenu);
    fileActions.append(action_quickadd);
    fileActions.append(action_add);
    folderActions.append(action_open);

    //菜单项的点击响应
    connect(action_quickadd,&QAction::triggered,[this](bool){
        QString path = nodeStack.join(" ");
        if(!path.isEmpty())
            path += " ";
        emit quickAdd(resourceFolder,path+this->currentItem()->text(),this->currentItem()->data(Qt::UserRole+1).toString());
    });
    connect(action_add,&QAction::triggered,[this](bool){
        QString path = nodeStack.join(" ");
        if(!path.isEmpty())
            path += " ";
        emit quickAdd(resourceFolder,path+this->currentItem()->text(),this->currentItem()->data(Qt::UserRole+1).toString());
    });
    connect(action_open,&QAction::triggered,[this](bool){
        emit itemDoubleClicked(currentItem());
    });



    //连接信号（双击、移过）
    connect(this,&QListWidget::itemDoubleClicked,[this](QListWidgetItem *item){
        if(item->data(Qt::UserRole).toInt()==0){
            //停止当前播放
            playingRow = -1;
            item->setIcon(QIcon(":/Sound/stop.png"));
            QBKAudio::getInstance()->stop(1);

            //改变子节点栈
            if(item->text()=="..")
                nodeStack.removeLast();
            else
                nodeStack.append(item->text());
            //加载子节点
            this->_load();
        }
        else{
            if(this->playingRow==this->row(item)){  //双击正在播放的项目
                playingRow = -1;
                item->setIcon(QIcon(":/Sound/stop.png"));
                QBKAudio::getInstance()->stop(1);
            }else if(this->playingRow==-1){ //没有在播放的项目
                playingRow = this->row(item);
                item->setIcon(QIcon(":/Sound/play.png"));
                QBKAudio::getInstance()->playSound(projectPath+"/Data/"+resourceFolder+"/"+item->data(Qt::UserRole+1).toString(),1,100,-1,false,false);
            }else{  //又在播放的项目，双击的不是该项目
                this->item(playingRow)->setIcon(QIcon(":/Sound/stop.png"));
                playingRow = this->row(item);
                item->setIcon(QIcon(":/Sound/play.png"));
                QBKAudio::getInstance()->stop(1);
                QBKAudio::getInstance()->playSound(projectPath+"/Data/"+resourceFolder+"/"+item->data(Qt::UserRole+1).toString(),1,100,-1,false,false);
            }
        }


    });
}
SoundSelectDialog::~SoundSelectDialog()
{
    QBKAudio::getInstance()->stop(1);
    QBKAudio::getInstance()->unload(1);
}

void SoundSelectDialog::closeEvent(QCloseEvent *)
{

}

void SoundSelectDialog::contextMenuEvent(QContextMenuEvent *)
{
    QListWidgetItem* item = this->itemAt(mapFromGlobal(QCursor::pos()));
    if(item == nullptr)
        return;
    if(item->data(Qt::UserRole)==0)
        rMenu->exec(folderActions,QCursor::pos());
    else
        rMenu->exec(fileActions,QCursor::pos());
}

void SoundSelectDialog::load(const QString &path, const QString &folder, const QBkeDictionary &dic)
{
    //外部调用入口，设置一些常量
    //内部不使用此接口，使用_load()
    this->projectPath = path;
    this->resourceFolder = folder;
    this->dic = dic;

    this->_load();
}

void SoundSelectDialog::_load()
{
    //清空
    this->clear();

    //获得当前节点
    QBkeDictionary node = dic;
    foreach(QString name,nodeStack){
        node = node[name].toBkeDic();
    }

    //创建上级项目
    if(node!=dic){
        QListWidgetItem *item = new QListWidgetItem("..");
        item->setData(Qt::UserRole,0);
        this->addItem(item);
    }

    //遍历子节点，创建项目
    QBkeDictionary::const_iterator ite;
    for(ite = node.cbegin();ite!=node.cend();ite++){
        QListWidgetItem *item = new QListWidgetItem(ite.key());
        //是文件
        if(ite.value().isString()){
            item->setData(Qt::UserRole+1,ite.value().toString());   //文件路径
            QFile f(projectPath+"/Data/"+resourceFolder+"/"+ite.value().toString());
            //是否为占位文件
            if(f.exists()){
                item->setData(Qt::UserRole,1);  //0-folder 1-file 2-voidfile
                item->setIcon(QIcon(":/Sound/stop"));
            }
            else{
                item->setData(Qt::UserRole,2);  //0-folder 1-file 2-voidfile
                item->setTextColor(QColor("red"));
                item->setIcon(QIcon(":/Sound/stop"));
            }
        }
        //是文件夹
        else if(ite.value().isDic()){
            item->setData(Qt::UserRole,0);
        }
        //添加
        this->addItem(item);
    }
}
