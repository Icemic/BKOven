#include "ImageSelectDialog.h"
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QListWidgetItem>
#include <QMenu>
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QFileIconProvider>

ImageSelectDialog::ImageSelectDialog(QWidget *parent) :
    QListWidget(parent)
{
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->setMouseTracking(true);
    this->setIconSize(QSize(80,80));
    this->setVerticalScrollMode(QListWidget::ScrollPerPixel);
    this->setHorizontalScrollMode(QListWidget::ScrollPerPixel);
    this->setGridSize(QSize(85,85));
    this->setViewMode(QListWidget::IconMode);
    this->setUniformItemSizes(true);
    this->setResizeMode(QListView::Adjust);
    this->setMinimumHeight(420);
    //设置预览窗口
    preview = new QWidget();
    preview->move(0,0);
    preview->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Tool);
    preview->setFixedHeight(400);
    QVBoxLayout *vlayout = new QVBoxLayout(preview);
    vlayout->setContentsMargins(0,0,0,0);
    QLabel *previewLabel = new QLabel;
    vlayout->addWidget(previewLabel);

    //设置右键菜单
    rMenu = new QMenu(this);
    QAction* action_add = new QAction("添加",rMenu);
    QAction* action_open = new QAction("打开",rMenu);
    fileActions.append(action_add);
    folderActions.append(action_open);

    //菜单项的点击响应
    connect(action_add,&QAction::triggered,[this](bool checked){
        qDebug() << resourceFolder << this->currentItem()->text() << this->currentItem()->data(Qt::UserRole+1).toString() << endl;
        //emit imageSelected(item->data(Qt::UserRole+1).toString());
    });
    connect(action_open,&QAction::triggered,[this](bool){
        emit itemDoubleClicked(currentItem());
    });


    //连接信号（双击、移过）
    connect(this,&QListWidget::itemEntered,[this,previewLabel](QListWidgetItem* item){
        if(!showPreview)
            return;
        if(item->data(Qt::UserRole).toInt()!=1)
            return;

        QString filePath = projectPath+"/Data/"+resourceFolder+"/"+item->data(Qt::UserRole+1).toString();
        QPixmap pmp(filePath);
        pmp = pmp.scaled(400,480,Qt::KeepAspectRatio,Qt::SmoothTransformation);
        previewLabel->setPixmap(pmp);
        preview->setFixedSize(pmp.size());
        QPoint pos = this->mapToGlobal(this->pos());
        pos.setX(pos.x()+this->width());
        preview->move(pos);
        preview->show();
    });

    connect(this,&QListWidget::itemDoubleClicked,[=](QListWidgetItem *item){
        if(item->data(Qt::UserRole).toInt()!=0)
            return;

        //改变子节点栈
        if(item->text()=="..")
            nodeStack.removeLast();
        else
            nodeStack.append(item->text());

        //加载子节点
        this->_load();
    });
}

ImageSelectDialog::~ImageSelectDialog()
{
    preview->deleteLater();
}

void ImageSelectDialog::closeEvent(QCloseEvent *e)
{
    preview->close();
    QListWidget::closeEvent(e);
}

void ImageSelectDialog::leaveEvent(QEvent *e)
{
    preview->hide();
    QListWidget::leaveEvent(e);
}

void ImageSelectDialog::contextMenuEvent(QContextMenuEvent *)
{
    QListWidgetItem* item = this->itemAt(mapFromGlobal(QCursor::pos()));
    if(item == nullptr)
        return;
    if(item->data(Qt::UserRole)==0)
        rMenu->exec(folderActions,QCursor::pos());
    else
        rMenu->exec(fileActions,QCursor::pos());
}

void ImageSelectDialog::load(const QString &path, const QString &folder, const QBkeDictionary &dic, bool showPreview)
{
    //外部调用入口，设置一些常量
    //内部不使用此接口，使用_load()
    this->showPreview = showPreview;
    this->projectPath = path;
    this->resourceFolder = folder;
    this->dic = dic;

    this->_load();
}

void ImageSelectDialog::_load()
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
        item->setIcon(QIcon(QPixmap(":/Image/folder").scaledToHeight(43)));
        this->addItem(item);
    }

    //遍历子节点，创建项目
    QBkeDictionary::const_iterator ite;
    for(ite = node.cbegin();ite!=node.cend();ite++){
        QListWidgetItem *item = new QListWidgetItem(ite.key());
        //是文件
        if(ite.value().isString()){
            item->setData(Qt::UserRole+1,ite.value().toString());
            QPixmap pix;
            //是否为占位文件
            if(pix.load(projectPath+"/Data/"+resourceFolder+"/"+ite.value().toString())){
                item->setData(Qt::UserRole,1);  //0-folder 1-file 2-voidfile
                item->setIcon(QIcon(pix.scaled(75,43,Qt::KeepAspectRatio,Qt::SmoothTransformation)));
            }
            else{
                item->setData(Qt::UserRole,2);  //0-folder 1-file 2-voidfile
                item->setIcon(QIcon(QPixmap(":/Image/folder").scaledToHeight(43)));
            }
        }
        //是文件夹
        else if(ite.value().isDic()){
            item->setData(Qt::UserRole,0);
            item->setIcon(QIcon(QPixmap(":/Image/folder").scaledToHeight(43)));
        }
        //添加
        this->addItem(item);
    }
}
