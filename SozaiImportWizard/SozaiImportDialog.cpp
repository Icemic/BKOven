#include "SozaiImportDialog.h"
#include "ui_SozaiImportDialog.h"
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

    //初始化预览框

    QGraphicsScene *scene_bg = new QGraphicsScene();
    ui->graphicsView_bg->setScene(scene_bg);
    ui->graphicsView_bg->setAlignment(Qt::AlignCenter);
    ui->graphicsView_bg->setBackgroundBrush(QBrush());
    ui->graphicsView_bg->setAutoFillBackground(true);
    ui->originList_bg->setSortingEnabled(true);
    ui->targetList_bg->setSortingEnabled(true);



    //originList选中项改变
    connect(ui->originList_bg,&QListWidget::currentTextChanged,[=](const QString &text){
        QGraphicsView *view = ui->graphicsView_bg;
        view->scene()->clear();
        view->scene()->addPixmap(QPixmap(projectDataPath+"background/"+text)
                                                .scaled(view->width()-2,view->height()-2,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    });

    //targetList选中项改变
    connect(ui->targetList_bg,&QListWidget::currentItemChanged,[=](QListWidgetItem* item){
        if(item->textColor().red()==255)
            return;

        QGraphicsView *view = ui->graphicsView_bg;
        view->scene()->clear();
        view->scene()->addPixmap(QPixmap(projectDataPath+"background/"+item->data(Qt::UserRole).toString())
                                                .scaled(view->width()-2,view->height()-2,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    });

    //点击导入按钮
    connect(ui->importButton_bg,&QPushButton::clicked,[=](){
        //判断originList当前row是否合法
        if(ui->originList_bg->currentRow()<0 || ui->originList_bg->currentRow() >= ui->originList_bg->count())
            return;

        QString fileNamePath = ui->originList_bg->currentItem()->text();

        QListWidgetItem *item = new QListWidgetItem(fileNamePath);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setData(Qt::UserRole,fileNamePath);                               //真正的路径信息存储在 Qt::UserRole 中, text 为别名。
        ui->targetList_bg->addItem(item);
        ui->targetList_bg->editItem(item);
        ui->originList_bg->takeItem(ui->originList_bg->currentRow());
    });

    //点击移除按钮
    connect(ui->removeButton_bg,&QPushButton::clicked,[=](){
        //判断targetList当前row是否合法
        if(ui->targetList_bg->currentRow()<0 || ui->targetList_bg->currentRow() >= ui->targetList_bg->count())
            return;

        //若文件并不存在，则不向originList中添加
        if(ui->targetList_bg->currentItem()->textColor().red()!=255)    //为红色（==255）表示文件不存在
            ui->originList_bg->addItem(ui->targetList_bg->currentItem()->data(Qt::UserRole).toString());
        ui->targetList_bg->takeItem(ui->targetList_bg->currentRow());
    });

    //点击占位按钮
    connect(ui->addButton_bg,&QPushButton::clicked,[=](){
        QListWidgetItem *item = new QListWidgetItem();
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setData(Qt::UserRole,"");
        item->setTextColor(QColor("red"));
        ui->targetList_bg->addItem(item);
        ui->targetList_bg->editItem(item);
    });



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
    QBkeArray targetData_bg = doc["background"].toBkeArray();
    QColor color;
    color.setRed(255);
    foreach(QBkeVariable data, targetData_bg){
        QListWidgetItem *item = new QListWidgetItem(data[0].toString());
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setData(Qt::UserRole,data[1].toString());

        //移除originList中已经添加到targetList的项目，并以此判断targetList中文件是否存在。
        //不存在则设置文字为红色
        bool exist = bgFileList.removeOne(data[1].toString());
        if(!exist) item->setTextColor(color);

        //向targetList逐项添加
        ui->targetList_bg->addItem(item);
    }
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
    QBkeVariable doc_bg;
    int I_bg = ui->targetList_bg->count();
    for(int i_bg=0;i_bg<I_bg;i_bg++){
        QListWidgetItem* item = ui->targetList_bg->item(i_bg);
        QBkeVariable itemVar;
//        itemVar.append(item->text());
//        itemVar.append(item->data(Qt::UserRole).toString());
//        doc_bg.append(itemVar);
        itemVar[0] = item->text();
        itemVar[1] = item->data(Qt::UserRole).toString();
        doc_bg[i_bg] = itemVar;
    }
    doc["background"] = doc_bg;
}
