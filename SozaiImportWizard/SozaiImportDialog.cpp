#include "SozaiImportDialog.h"
#include "ui_SozaiImportDialog.h"
#include "../shared/vshack.h"
#include <iostream>
#include <functional>
#include <QDir>
#include <QMessageBox>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QOpenGLWidget>
#include <QDebug>

SozaiImportDialog::SozaiImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SozaiImportDialog)
{
    ui->setupUi(this);



    init_bg();



    //点击确定按钮
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(_exportSozaiConfig()));
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(accept()));

    //点击取消按钮
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(reject()));

}

SozaiImportDialog::~SozaiImportDialog()
{
    delete ui;
}

void SozaiImportDialog::init_bg()
{
    //初始化预览框
    QGraphicsScene *scene_bg = new QGraphicsScene();
    ui->graphicsView_bg->setScene(scene_bg);
    ui->graphicsView_bg->setAlignment(Qt::AlignCenter);
    //ui->graphicsView_bg->setBackgroundBrush(QBrush());
    ui->graphicsView_bg->setAutoFillBackground(true);
    ui->originList_bg->setSortingEnabled(true);

    //0-root, 1-sub, 2-end without value, 3-end with value

    QTreeWidgetItem* item = new QTreeWidgetItem(QStringList(tr("背景图片素材库")));
    item->setData(0,Qt::UserRole,0);
    item->setTextColor(1,QColor("red"));
    ui->targetList_bg->addTopLevelItem(item);
    ui->targetList_bg->setCurrentItem(item);
    ui->targetList_bg->header()->setSortIndicator(0,Qt::AscendingOrder);
    ui->targetList_bg->setHeaderHidden(false);



    //添加子分类
    connect(ui->addClassButton_bg,&QPushButton::clicked,[=](){
        if(ui->targetList_bg->currentItem() == nullptr)
            return;

        if(ui->targetList_bg->currentItem()->data(0,Qt::UserRole)==3){
            QMessageBox::warning(this,tr("素材导入"),tr("请先移除该分类下已经导入的素材，方可添加子分类。\n（只有最末级子分类可以导入素材）"));
            return;
        }

        QTreeWidgetItem* child = new QTreeWidgetItem();
        child->setData(0,Qt::UserRole,2);
        child->setFlags(child->flags() | Qt::ItemIsEditable);
        child->setText(1,tr("拖拽到此处导入素材"));
        child->setTextColor(1,QColor("red"));


        QTreeWidgetItem* parent = ui->targetList_bg->currentItem();
        if(parent!=ui->targetList_bg->topLevelItem(0)){
            parent->setData(0,Qt::UserRole,1);
            parent->setText(1,"");
        }
        ui->targetList_bg->currentItem()->addChild(child);
        ui->targetList_bg->currentItem()->setExpanded(true);
        child->setText(0,tr(""));
        ui->targetList_bg->editItem(child);
    });

    //移除子分类
    connect(ui->removeClassButton_bg,&QPushButton::clicked,[=](){
        if(ui->targetList_bg->currentItem() == nullptr)
            return;
        if(ui->targetList_bg->currentItem()->data(0,Qt::UserRole)==0)
            return;

        //得到父子包含的素材文件路径列表
        QStringList fileList = getFileNamesInChildren(ui->targetList_bg->currentItem());

        //从targetList中删除（包含子类）
        ui->targetList_bg->currentItem()->parent()->takeChild(ui->targetList_bg->currentItem()->parent()
                                                      ->indexOfChild(ui->targetList_bg->currentItem()));

        //将删除的素材放回originList
        ui->originList_bg->addItems(fileList);

        //判断父级是否有子项
        QTreeWidgetItem* parent = ui->targetList_bg->currentItem()->parent();
        if(parent->childCount()==0){
            parent->setText(1,tr("拖拽到此处导入素材"));
            parent->setTextColor(1,QColor("red"));
            parent->setData(0,Qt::UserRole,2);
        }
    });

    //拖拽释放事件
    connect(ui->targetList_bg,&SozaiTreeWidget::dropped,[=](QTreeWidgetItem* target, QMap<int,QVariant> data){
        if(target->data(0,Qt::UserRole)==2){
            target->setText(1,data.value(0).toString());
            target->setTextColor(1,QColor("black"));
            ui->originList_bg->takeItem(ui->originList_bg->currentRow());
            target->setData(0,Qt::UserRole,3);
        }
        else if(target->data(0,Qt::UserRole)==3){
            ui->originList_bg->addItem(target->text(1));
            ui->originList_bg->takeItem(ui->originList_bg->currentRow());
            target->setText(1,data.value(0).toString());
        }
        else{
            QMessageBox::warning(this,tr("素材导入"),tr("请将素材文件拖动到最末级子分类上，只有最末级子分类可以导入素材！"));
        }
    });


    //originList选中项改变
    connect(ui->originList_bg,&QListWidget::currentTextChanged,[=](const QString &text){
        QGraphicsView *view = ui->graphicsView_bg;
        view->scene()->clear();
        view->scene()->addPixmap(QPixmap(projectDataPath+"background/"+text)
                                                .scaled(view->width()-2,view->height()-2,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    });

    //targetList选中项改变
    connect(ui->targetList_bg,&SozaiTreeWidget::currentItemChanged,[=](QTreeWidgetItem* current,QTreeWidgetItem*){
        if(current->data(0,Qt::UserRole)==3){
            QGraphicsView *view = ui->graphicsView_bg;
            view->scene()->clear();
            view->scene()->addPixmap(QPixmap(projectDataPath+"background/"+current->text(1))
                                                    .scaled(view->width()-2,view->height()-2,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        }
    });

    //禁止第二栏编辑
    connect(ui->targetList_bg,&SozaiTreeWidget::itemDoubleClicked,[this](QTreeWidgetItem* item,int col){
        if(col==1)
            ui->targetList_bg->editItem(item,0);
    });

    //防止空内容、消左右空格、查重名
    connect(ui->targetList_bg,&SozaiTreeWidget::itemChanged,[=](QTreeWidgetItem* item,int){

        if(item->data(0,Qt::UserRole)==0)
            return;

        QTimer::singleShot(0,[=](){
            //不能为空
            if(item->text(0).trimmed().isEmpty()){

            }

            //消左右空格
            if(item->text(0).trimmed()!=item->text(0)){
                item->setText(0,item->text(0).trimmed());
                return;
            }
            int I = item->parent()->childCount();
            int i = 0;
            for(i;i<I;i++){
                QTreeWidgetItem* item2 = item->parent()->child(i);
                if(item2!=item && item2->text(0)==item->text(0)){
                    QMessageBox::warning(this,tr("素材导入"),tr("同一父分类下的子分类不能重名哦～"));
                    item->setText(0,"");
                    ui->targetList_bg->editItem(item,0);
                }
            }
        });

    });

    //复制
    connect(ui->copyClassButton_bg,&QPushButton::clicked,[=](){
        QTreeWidgetItem* item = ui->targetList_bg->currentItem();
        if(item->data(0,Qt::UserRole)==0)
            return;

        QTreeWidgetItem* item2 = item->clone();

        //清除全部子项中的导入素材
        std::function<void(QTreeWidgetItem*)> clear_recursive;
        clear_recursive = [&clear_recursive](QTreeWidgetItem* parent){
            int i,I = parent->childCount();
            for(i=0;i<I;i++){
                QTreeWidgetItem* item = parent->child(i);
                if(item->data(0,Qt::UserRole)==3){
                    item->setText(1,tr("拖拽到此处导入素材"));
                    item->setTextColor(1,QColor("red"));
                    item->setData(0,Qt::UserRole,2);
                }
                else if(item->data(0,Qt::UserRole)==1){
                    clear_recursive(item);
                }
            }
        };
        clear_recursive(item2);


        item2->setText(0,item2->text(0)+tr("_副本"));
        item->parent()->addChild(item2);

        //展开复制出的新项目
        std::function<void(QTreeWidgetItem*)> expand_recursive;
        expand_recursive = [&expand_recursive,this](QTreeWidgetItem* parent){
            ui->targetList_bg->expandItem(parent);
            int i,I = parent->childCount();
            for(i=0;i<I;i++){
                QTreeWidgetItem* item = parent->child(i);
                if(item->data(0,Qt::UserRole)==1){
                    expand_recursive(item);
                }
            }
        };
        expand_recursive(item2);

    });

    //清除当前
    connect(ui->clearButton_bg,&QPushButton::clicked,[=](){
        QTreeWidgetItem* item = ui->targetList_bg->currentItem();
        if(item->data(0,Qt::UserRole)!=3)
            return;

        ui->originList_bg->addItem(item->text(1));

        item->setText(1,tr("拖拽到此处导入素材"));
        item->setTextColor(1,QColor("red"));
        item->setData(0,Qt::UserRole,2);
    });


}

QStringList SozaiImportDialog::getFileNamesInChildren(QTreeWidgetItem *parent)
{
    QStringList list;
    int I = parent->childCount();
    int i;


    if(parent->data(0,Qt::UserRole)==3)   //被删除的是最末级子类
    {
            list << parent->text(1);
    }
    else    //被删除的是非最末级子类
    {
        for(i = 0;i < I;i++){
            QTreeWidgetItem* child = parent->child(i);
            if(child->childCount()!=0)
                list += getFileNamesInChildren(child);
            if(child->textColor(1).red()!=255)
                list << child->text(1);
        }
    }
    return list;
}


bool SozaiImportDialog::execWithPath(const QString &path)
{
    projectDataPath = path + "/Data/";

    /*读取文件目录，获得文件列表（OriginList）*/

    //错误处理
    QDir dir(path);
    if(!dir.exists()){
        QMessageBox::warning(this,tr("素材导入"),tr("错误的路径"));
        return false;
    }

    //读取背景文件
    dir.cd("Data/background");
    QStringList imageNameFilters;
    imageNameFilters << "*.bmp" << "*.png" << "*.jpg";
    QStringList bgFileList = this->recursiveSearch(dir.absolutePath()+"/",dir.absolutePath()+"/", imageNameFilters);

    //读取立绘文件
    //....




    /*读取配置文件中的文件列表（targetList）*/
    //加载当前配置
    QFile f(path+"/config/sozai.bkpsr");
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    doc.loadFromBinary(f.readAll());
    f.close();


    /*消重复、判断文件存在性、向QListView添加数据*/
    //背景图片
    QBkeDictionary targetData_bg = doc["background"].toBkeDic();
    std::function<void(const QBkeDictionary&, QTreeWidgetItem*)> bgRead_recursive;
    bgRead_recursive = [this,&bgRead_recursive,&bgFileList](const QBkeDictionary &data, QTreeWidgetItem* parent){
        QBkeDictionary::const_iterator ite;
        for(ite = data.begin();ite!=data.end();ite++){
            QBkeVariable member = data[ite.key()];
            if(member.isString()){
                QTreeWidgetItem* item = new QTreeWidgetItem();
                item->setFlags(item->flags() | Qt::ItemIsEditable);
                item->setText(0,ite.key());
                if(member.toString().isEmpty()){
                    item->setData(0,Qt::UserRole,2);
                    item->setText(1,tr("拖拽到此处导入素材"));
                    item->setTextColor(1,QColor("red"));
                }else{
                    item->setText(1,member.toString());
                    bool exist = bgFileList.removeOne(member.toString());
                    if(!exist){
                        item->setTextColor(1,QColor("red"));
                        item->setData(0,Qt::UserRole,2);
                    }
                    else{
                        item->setData(0,Qt::UserRole,3);
                    }
                }
                parent->addChild(item);
            }
            else if(member.isDic()){
                QTreeWidgetItem* pItem = new QTreeWidgetItem();
                pItem->setData(0,Qt::UserRole,1);
                pItem->setText(0,ite.key());
                pItem->setFlags(pItem->flags() | Qt::ItemIsEditable);
                parent->addChild(pItem);
                bgRead_recursive(member.toBkeDic(),pItem);
            }
        }
    };
    bgRead_recursive(targetData_bg,ui->targetList_bg->topLevelItem(0));
    //向originList一次性添加
    ui->originList_bg->addItems(bgFileList);
    //展开树形结构
    ui->targetList_bg->expandAll();

    //立绘图片
    //....




    //启动窗口
    return this->exec();
}

QStringList SozaiImportDialog::recursiveSearch(const QString &path, const QString &relativePath, const QStringList &nameFilters)
{
    QStringList fileList;
    QDir dir(path);
    QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::CaseSensitive);
    foreach(QFileInfo item, dirs){
        fileList += this->recursiveSearch(item.absoluteFilePath(),relativePath,nameFilters);
    }

    QFileInfoList files = dir.entryInfoList(nameFilters,QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::CaseSensitive);
    foreach(QFileInfo file, files){
        fileList << file.absoluteFilePath().replace(relativePath,"");
    }
    return fileList;
}

void SozaiImportDialog::_exportSozaiConfig()
{
    doc.setVoid();  //清空
    //导出背景图片信息
    doc["background"] = _exportSozaiConfig_recursive(ui->targetList_bg->topLevelItem(0));
}

QBkeVariable SozaiImportDialog::_exportSozaiConfig_recursive(QTreeWidgetItem *parent)
{
    QBkeVariable root = QBkeVariable::dic();
    int i,I=parent->childCount();
    for(i=0;i<I;i++){
        QTreeWidgetItem* child = parent->child(i);
        //末端节点
        int type = child->data(0,Qt::UserRole).toInt();
        if(type==2||type==3){
            root[child->text(0)] = (type==2)?"":child->text(1);
        }
        else{
            root[child->text(0)] = _exportSozaiConfig_recursive(child);
        }
    }
    return root;
}
