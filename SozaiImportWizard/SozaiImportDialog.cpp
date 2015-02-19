#include "SozaiImportDialog.h"
#include "ui_SozaiImportDialog.h"
#include "../shared/vshack.h"

#include <QDir>
#include <QMessageBox>
#include <QGraphicsView>
#include <QGraphicsScene>
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
    ui->graphicsView_bg->setBackgroundBrush(QBrush());
    ui->graphicsView_bg->setAutoFillBackground(true);
    ui->originList_bg->setSortingEnabled(true);


    QTreeWidgetItem* item = new QTreeWidgetItem(QStringList(tr("背景图片素材库")),0);
    item->setTextColor(1,QColor("red"));
    ui->targetList_bg->addTopLevelItem(item);
    ui->targetList_bg->setCurrentItem(item);
    ui->targetList_bg->setSortingEnabled(true);


    //添加子分类

    connect(ui->addClassButton_bg,&QPushButton::clicked,[=](){
        if(ui->targetList_bg->currentItem() == nullptr)
            return;

        if(ui->targetList_bg->currentItem()->textColor(1).red()!=255){
            QMessageBox::warning(this,"素材导入",tr("请先移除改分类下已经导入的素材，方可添加子分类。\n（只有最末级子分类可以导入素材）"));
            return;
        }

        QTreeWidgetItem* child = new QTreeWidgetItem(1);
        child->setFlags(child->flags() | Qt::ItemIsEditable);
        child->setText(1,tr("拖拽到此处导入素材"));
        child->setTextColor(1,QColor("red"));



        ui->targetList_bg->currentItem()->setText(1,"");
        ui->targetList_bg->currentItem()->addChild(child);
        ui->targetList_bg->currentItem()->setExpanded(true);
        child->setText(0,tr(""));
        ui->targetList_bg->editItem(child);
//        this->isEditting = true;
    });

    //移除子分类
    connect(ui->removeClassButton_bg,&QPushButton::clicked,[=](){
        if(ui->targetList_bg->currentItem() == nullptr)
            return;
        if(ui->targetList_bg->currentItem()->type()==0)
            return;

        //得到父子包含的素材文件路径列表
        QStringList fileList = getFileNamesInChildren(ui->targetList_bg->currentItem());

        //从targetList中删除（包含子类）
        ui->targetList_bg->currentItem()->parent()->takeChild(ui->targetList_bg->currentItem()->parent()
                                                      ->indexOfChild(ui->targetList_bg->currentItem()));

        //将删除的素材放回originList
        ui->originList_bg->addItems(fileList);


    });

    //拖拽释放事件
    connect(ui->targetList_bg,&SozaiTreeWidget::dropped,[=](QTreeWidgetItem* target, QMap<int,QVariant> data){
        if(target->textColor(1).red()==255 && target->childCount()==0 && target->type()==1){
            target->setText(1,data.value(0).toString());
            target->setTextColor(1,QColor("black"));
            ui->originList_bg->takeItem(ui->originList_bg->currentRow());
        }else{
            QMessageBox::warning(this,"素材导入",tr("请将素材文件拖动到最末级子分类上，只有最末级子分类可以导入素材！"));
        }
    });


    //originList选中项改变
    connect(ui->originList_bg,&QListWidget::currentTextChanged,[=](const QString &text){
        QGraphicsView *view = ui->graphicsView_bg;
        view->scene()->clear();
        view->scene()->addPixmap(QPixmap(projectDataPath+"background/"+text)
                                                .scaled(view->width()-2,view->height()-2,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    });

    //禁止第二栏编辑
    connect(ui->targetList_bg,&SozaiTreeWidget::itemDoubleClicked,[this](QTreeWidgetItem* item,int col){
        if(col==1)
            ui->targetList_bg->editItem(item,0);
    });

    connect(ui->targetList_bg,&SozaiTreeWidget::itemChanged,[=](QTreeWidgetItem* item,int col){
        if(item->type()==0)
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
                qDebug() << item2 << item << item2->text(0) << item->text(0) <<endl;
                if(item2!=item && item2->text(0)==item->text(0)){
                    QMessageBox::warning(this,"素材导入",tr("同一父分类下的子分类不能重名哦～"));
                    item->setText(0,"");
                    ui->targetList_bg->editItem(item,0);
                }
            }
        });

    });



}

QStringList SozaiImportDialog::getFileNamesInChildren(QTreeWidgetItem *parent)
{
    QStringList list;
    int I = parent->childCount();
    int i = 0;


    if(I == 0 && parent->textColor(1).red()!=255)   //被删除的是最末级子类
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
//    QBkeArray targetData_bg = doc["background"].toBkeArray();
//    QColor color;
//    color.setRed(255);
//    foreach(QBkeVariable data, targetData_bg){
//        QListWidgetItem *item = new QListWidgetItem(data[0].toString());
//        item->setFlags(item->flags() | Qt::ItemIsEditable);
//        item->setData(Qt::UserRole,data[1].toString());

//        //移除originList中已经添加到targetList的项目，并以此判断targetList中文件是否存在。
//        //不存在则设置文字为红色
//        bool exist = bgFileList.removeOne(data[1].toString());
//        if(!exist) item->setTextColor(color);

//        //向targetList逐项添加
//        ui->targetList_bg->addItem(item);
//    }
    //向originList一次性添加
    ui->originList_bg->addItems(bgFileList);




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
//    QBkeVariable doc_bg;
//    int I_bg = ui->targetList_bg->count();
//    for(int i_bg=0;i_bg<I_bg;i_bg++){
//        QListWidgetItem* item = ui->targetList_bg->item(i_bg);
//        QBkeVariable itemVar;
//        itemVar[0] = item->text();
//        itemVar[1] = item->data(Qt::UserRole).toString();
//        doc_bg[i_bg] = itemVar;
//    }
//    doc["background"] = doc_bg;
}
