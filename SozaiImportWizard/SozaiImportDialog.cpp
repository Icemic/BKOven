#include "SozaiImportDialog.h"
#include "ui_SozaiImportDialog.h"
#include "../shared/vshack.h"
#include <iostream>
#include <functional>
#include <QDir>
#include <QMessageBox>
#include <QtMultimedia>
#include "BKAudio.h"
#include "taglib/taglib.h"
#include "taglib/fileref.h"
#include "taglib/tpropertymap.h"
#include <QDebug>

SozaiImportDialog::SozaiImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SozaiImportDialog)
{
    ui->setupUi(this);



    init_image(ui->graphicsView_bg, ui->originList_bg, ui->targetList_bg,
            ui->addClassButton_bg, ui->removeClassButton_bg, ui->copyClassButton_bg, ui->clearButton_bg,
               tr("背景图片素材库"), "background");
    init_image(ui->graphicsView_ch, ui->originList_ch, ui->targetList_ch,
            ui->addClassButton_ch, ui->removeClassButton_ch, ui->copyClassButton_ch, ui->clearButton_ch,
               tr("立绘图片素材库"), "character");
    init_image(ui->graphicsView_item, ui->originList_item, ui->targetList_item,
            ui->addClassButton_item, ui->removeClassButton_item, ui->copyClassButton_item, ui->clearButton_item,
               tr("物件图片素材库"), "item");
    init_sound(ui->metaDataWidget_bgm, ui->metaDataEditButton_bgm, ui->metaDataResetButton_bgm,
               ui->playProcessSlider_bgm, ui->playPostionLabel_bgm, ui->playButton_bgm, ui->pauseButton_bgm, ui->stopButton_bgm,
               ui->originList_bgm, ui->targetList_bgm,
               ui->addClassButton_bgm, ui->removeClassButton_bgm, ui->copyClassButton_bgm, ui->clearButton_bgm,
               "背景音乐素材库", "bgm");


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

//界面初始化函数，图片类tab通用
void SozaiImportDialog::init_image(QGraphicsView* graphicsView, QListWidget* originList, SozaiTreeWidget* targetList,
                                   QPushButton* addClassButton, QPushButton* removeClassButton, QPushButton* copyClassButton, QPushButton* clearButton,
                                   const QString &topItemName, const QString &folderName)
{
    //初始化预览框
    QGraphicsScene *scene = new QGraphicsScene();
    graphicsView->setScene(scene);
    graphicsView->setAlignment(Qt::AlignCenter);
    //ui->graphicsView_bg->setBackgroundBrush(QBrush());
    graphicsView->setAutoFillBackground(true);
    originList->setSortingEnabled(true);

    //0-root, 1-sub, 2-end without value, 3-end with value

    QTreeWidgetItem* item = new QTreeWidgetItem(QStringList(topItemName));
    item->setData(0,Qt::UserRole,0);
    item->setTextColor(1,QColor("red"));
    targetList->addTopLevelItem(item);
    targetList->setCurrentItem(item);
    targetList->header()->setSortIndicator(0,Qt::AscendingOrder);
    targetList->setHeaderHidden(false);



    //添加子分类
    connect(addClassButton,&QPushButton::clicked,[=](){
        if(targetList->currentItem() == nullptr)
            return;

        if(targetList->currentItem()->data(0,Qt::UserRole)==3){
            QMessageBox::warning(this,tr("素材导入"),tr("请先移除该分类下已经导入的素材，方可添加子分类。\n（只有最末级子分类可以导入素材）"));
            return;
        }

        QTreeWidgetItem* child = new QTreeWidgetItem();
        child->setData(0,Qt::UserRole,2);
        child->setFlags(child->flags() | Qt::ItemIsEditable);
        child->setText(1,tr("拖拽到此处导入素材"));
        child->setTextColor(1,QColor("red"));


        QTreeWidgetItem* parent = targetList->currentItem();
        if(parent!=targetList->topLevelItem(0)){
            parent->setData(0,Qt::UserRole,1);
            parent->setText(1,"");
        }
        targetList->currentItem()->addChild(child);
        targetList->currentItem()->setExpanded(true);
        child->setText(0,tr(""));
        targetList->editItem(child);
    });

    //移除子分类
    connect(removeClassButton,&QPushButton::clicked,[=](){
        if(targetList->currentItem() == nullptr)
            return;
        if(targetList->currentItem()->data(0,Qt::UserRole)==0)
            return;

        //得到父子包含的素材文件路径列表
        QStringList fileList = getFileNamesInChildren(targetList->currentItem());

        //从targetList中删除（包含子类）
        targetList->currentItem()->parent()->takeChild(targetList->currentItem()->parent()
                                                      ->indexOfChild(targetList->currentItem()));

        //将删除的素材放回originList
        originList->addItems(fileList);

        //判断父级是否有子项
        QTreeWidgetItem* parent = targetList->currentItem()->parent();
        if(parent->childCount()==0){
            parent->setText(1,tr("拖拽到此处导入素材"));
            parent->setTextColor(1,QColor("red"));
            parent->setData(0,Qt::UserRole,2);
        }
    });

    //拖拽释放事件
    connect(targetList,&SozaiTreeWidget::dropped,[=](QTreeWidgetItem* target, QMap<int,QVariant> data){
        if(target->data(0,Qt::UserRole)==2){
            target->setText(1,data.value(0).toString());
            target->setTextColor(1,QColor("black"));
            originList->takeItem(originList->currentRow());
            target->setData(0,Qt::UserRole,3);
        }
        else if(target->data(0,Qt::UserRole)==3){
            originList->addItem(target->text(1));
            originList->takeItem(originList->currentRow());
            target->setText(1,data.value(0).toString());
        }
        else{
            QMessageBox::warning(this,tr("素材导入"),tr("请将素材文件拖动到最末级子分类上，只有最末级子分类可以导入素材！"));
        }
    });


    //originList选中项改变
    connect(originList,&QListWidget::currentTextChanged,[=](const QString &text){
        QGraphicsView *view = graphicsView;
        view->scene()->clear();
        view->scene()->addPixmap(QPixmap(projectDataPath+folderName+"/"+text)
                                                .scaled(view->width()-2,view->height()-2,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    });

    //targetList选中项改变
    connect(targetList,&SozaiTreeWidget::currentItemChanged,[=](QTreeWidgetItem* current,QTreeWidgetItem*){
        if(current->data(0,Qt::UserRole)==3){
            QGraphicsView *view = graphicsView;
            view->scene()->clear();
            view->scene()->addPixmap(QPixmap(projectDataPath+folderName+"/"+current->text(1))
                                                    .scaled(view->width()-2,view->height()-2,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        }
    });

    //禁止第二栏编辑
    connect(targetList,&SozaiTreeWidget::itemDoubleClicked,[=](QTreeWidgetItem* item,int col){
        if(col==1)
            targetList->editItem(item,0);
    });

    //防止空内容、消左右空格、查重名
    connect(targetList,&SozaiTreeWidget::itemChanged,[=](QTreeWidgetItem* item,int){

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
                    targetList->editItem(item,0);
                }
            }
        });

    });

    //复制
    connect(copyClassButton,&QPushButton::clicked,[=](){
        QTreeWidgetItem* item = targetList->currentItem();
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
        expand_recursive = [=,&expand_recursive](QTreeWidgetItem* parent){
            targetList->expandItem(parent);
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
    connect(clearButton,&QPushButton::clicked,[=](){
        QTreeWidgetItem* item = targetList->currentItem();
        if(item->data(0,Qt::UserRole)!=3)
            return;

        originList->addItem(item->text(1));

        item->setText(1,tr("拖拽到此处导入素材"));
        item->setTextColor(1,QColor("red"));
        item->setData(0,Qt::UserRole,2);
    });


}

//界面初始化函数，图片类tab通用
void SozaiImportDialog::init_sound(QTableWidget* metaDataWidget, QPushButton* metaDataEditButton, QPushButton* metaDataResetButton,
                                   QSlider* playProcessSlider, QLabel* playPostionLabel, QPushButton* playButton, QPushButton* pauseButton, QPushButton* stopButton,
                                   QListWidget* originList, SozaiTreeWidget* targetList,
                                   QPushButton* addClassButton, QPushButton* removeClassButton, QPushButton* copyClassButton, QPushButton* clearButton,
                                   const QString &topItemName, const QString &folderName)
{
    //初始化预览框
    metaDataWidget->item(0,0)->setFlags(metaDataWidget->item(0,0)->flags() &~ Qt::ItemIsEnabled);
    metaDataWidget->setEnabled(false);


    //QMediaPlayer* mediaPlayer = new QMediaPlayer();

    //播放进度滑条
    QTimer *playProcessTimer = new QTimer();
    connect(playProcessTimer,&QTimer::timeout,[=](){
        qint64 pos = QBKAudio::getInstance()->tell(0);
        playProcessSlider->setRange(0,360000);
        playProcessSlider->setValue(pos);
        playPostionLabel->setText(QString("%1 / %2").arg(QTime::fromMSecsSinceStartOfDay(pos).toString("mm:ss"),QTime::fromMSecsSinceStartOfDay(360000).toString("mm:ss")));
    });
    playProcessTimer->start(1000);

    //窗口关闭时停止并销毁
    connect(this,&SozaiImportDialog::destroyed,[=](){
//        mediaPlayer->stop();
//        mediaPlayer->deleteLater();
        playProcessTimer->stop();
        playProcessTimer->deleteLater();
        QBKAudio::getInstance()->stop(0);
        QBKAudio::getInstance()->unload(0);
    });

    //元数据显示
//    connect(mediaPlayer,static_cast<void(QMediaPlayer::*)()>(&QMediaPlayer::metaDataChanged),[=](){
//        metaDataWidget->item(0,0)->setText(mediaPlayer->currentMedia().canonicalUrl().fileName());
//        metaDataWidget->item(1,0)->setText(mediaPlayer->metaData("Title").toString());
//        metaDataWidget->item(2,0)->setText(mediaPlayer->metaData("AlbumTitle").toString());
//        metaDataWidget->item(3,0)->setText(mediaPlayer->metaData("ContributingArtist").toString());
//        metaDataWidget->item(4,0)->setText(mediaPlayer->metaData("Lyrics").toString());
//        metaDataWidget->item(5,0)->setText(mediaPlayer->metaData("Composer").toString());
//        metaDataWidget->item(6,0)->setText(mediaPlayer->metaData("AlbumArtist").toString());
//        metaDataWidget->item(7,0)->setText(mediaPlayer->metaData("Copyright").toString());
//    });

    //无法编辑元数据，按钮暂时隐藏
    metaDataEditButton->hide();
    metaDataResetButton->hide();

    //播放进度滑条
//    connect(mediaPlayer,&QMediaPlayer::positionChanged,[=](qint64 pos){
//        playProcessSlider->setRange(0,mediaPlayer->duration());
//        playProcessSlider->setValue(pos);
//        playPostionLabel->setText(QString("%1 / %2").arg(QTime::fromMSecsSinceStartOfDay(pos).toString("mm:ss"),QTime::fromMSecsSinceStartOfDay(mediaPlayer->duration()).toString("mm:ss")));
//    });

    //进度条拖动
    //connect(playProcessSlider,&QSlider::valueChanged,mediaPlayer,&QMediaPlayer::setPosition);
    connect(playProcessSlider,&QSlider::sliderMoved,[=](int pos){
        QBKAudio::getInstance()->seek(0,pos);
    });

    //播放、暂停、停止按钮
    connect(playButton,&QPushButton::clicked,[=](){
        QBKAudio::getInstance()->resume(0);
    });
    connect(pauseButton,&QPushButton::clicked,[=](){
        QBKAudio::getInstance()->pause(0);
    });
    connect(stopButton,&QPushButton::clicked,[=](){
        QBKAudio::getInstance()->stop(0);
    });

    QTreeWidgetItem* item = new QTreeWidgetItem(QStringList(topItemName));
    item->setData(0,Qt::UserRole,0);
    item->setTextColor(1,QColor("red"));
    targetList->addTopLevelItem(item);
    targetList->setCurrentItem(item);
    targetList->header()->setSortIndicator(0,Qt::AscendingOrder);
    targetList->setHeaderHidden(false);



    //添加子分类
    connect(addClassButton,&QPushButton::clicked,[=](){
        if(targetList->currentItem() == nullptr)
            return;

        if(targetList->currentItem()->data(0,Qt::UserRole)==3){
            QMessageBox::warning(this,tr("素材导入"),tr("请先移除该分类下已经导入的素材，方可添加子分类。\n（只有最末级子分类可以导入素材）"));
            return;
        }

        QTreeWidgetItem* child = new QTreeWidgetItem();
        child->setData(0,Qt::UserRole,2);
        child->setFlags(child->flags() | Qt::ItemIsEditable);
        child->setText(1,tr("拖拽到此处导入素材"));
        child->setTextColor(1,QColor("red"));


        QTreeWidgetItem* parent = targetList->currentItem();
        if(parent!=targetList->topLevelItem(0)){
            parent->setData(0,Qt::UserRole,1);
            parent->setText(1,"");
        }
        targetList->currentItem()->addChild(child);
        targetList->currentItem()->setExpanded(true);
        child->setText(0,tr(""));
        targetList->editItem(child);
    });

    //移除子分类
    connect(removeClassButton,&QPushButton::clicked,[=](){
        if(targetList->currentItem() == nullptr)
            return;
        if(targetList->currentItem()->data(0,Qt::UserRole)==0)
            return;

        //得到父子包含的素材文件路径列表
        QStringList fileList = getFileNamesInChildren(targetList->currentItem());

        //从targetList中删除（包含子类）
        targetList->currentItem()->parent()->takeChild(targetList->currentItem()->parent()
                                                      ->indexOfChild(targetList->currentItem()));

        //将删除的素材放回originList
        originList->addItems(fileList);

        //判断父级是否有子项
        QTreeWidgetItem* parent = targetList->currentItem()->parent();
        if(parent->childCount()==0){
            parent->setText(1,tr("拖拽到此处导入素材"));
            parent->setTextColor(1,QColor("red"));
            parent->setData(0,Qt::UserRole,2);
        }
    });

    //拖拽释放事件
    connect(targetList,&SozaiTreeWidget::dropped,[=](QTreeWidgetItem* target, QMap<int,QVariant> data){
        if(target->data(0,Qt::UserRole)==2){
            target->setText(1,data.value(0).toString());
            target->setTextColor(1,QColor("black"));
            originList->takeItem(originList->currentRow());
            target->setData(0,Qt::UserRole,3);
        }
        else if(target->data(0,Qt::UserRole)==3){
            originList->addItem(target->text(1));
            originList->takeItem(originList->currentRow());
            target->setText(1,data.value(0).toString());
        }
        else{
            QMessageBox::warning(this,tr("素材导入"),tr("请将素材文件拖动到最末级子分类上，只有最末级子分类可以导入素材！"));
        }
    });


    //originList选中项改变
    connect(originList,&QListWidget::currentTextChanged,[=](const QString &text){
//        mediaPlayer->setMedia(QUrl::fromLocalFile(projectDataPath+folderName+"/"+text));
        QBKAudio::getInstance()->playSound(projectDataPath+folderName+"/"+text,0,100,-1,false,false);
        QBKAudio::getInstance()->pause(0);
//        TagLib::FileName fileName((projectDataPath+folderName+"/"+text).toStdWString().c_str());
//        TagLib::FileRef f(fileName);
//        if(!f.file()->isValid()) return;
//        metaDataWidget->item(0,0)->setText(QString::fromStdWString(f.file()->name().wstr()));
//        metaDataWidget->item(1,0)->setText(QString::fromStdWString(f.tag()->title().toWString()));
//        metaDataWidget->item(2,0)->setText(QString::fromStdWString(f.tag()->album().toWString()));
//        metaDataWidget->item(3,0)->setText(QString::fromStdWString(f.tag()->artist().toWString()));
//        metaDataWidget->item(4,0)->setText(QString::fromStdWString(f.tag()->properties()["LYRICIST"][0].toWString()));
//        metaDataWidget->item(5,0)->setText(QString::fromStdWString(f.tag()->properties()["COMPOSER"][0].toWString()));
//        metaDataWidget->item(6,0)->setText(QString::fromStdWString(f.tag()->properties()["ALBUMARTIST"][0].toWString()));
//        metaDataWidget->item(7,0)->setText(QString::fromStdWString(f.tag()->properties()["COPYRIGHT"][0].toWString()));

    });

    //targetList选中项改变
    connect(targetList,&SozaiTreeWidget::currentItemChanged,[=](QTreeWidgetItem* current,QTreeWidgetItem*){
        if(current->data(0,Qt::UserRole)==3){
//            mediaPlayer->setMedia(QUrl::fromLocalFile(projectDataPath+folderName+"/"+current->text(1)));
            QBKAudio::getInstance()->playSound(projectDataPath+folderName+"/"+current->text(1),0,100,-1,false,false);
            QBKAudio::getInstance()->pause(0);
        }
    });

    //禁止第二栏编辑
    connect(targetList,&SozaiTreeWidget::itemDoubleClicked,[=](QTreeWidgetItem* item,int col){
        if(col==1)
            targetList->editItem(item,0);
    });

    //防止空内容、消左右空格、查重名
    connect(targetList,&SozaiTreeWidget::itemChanged,[=](QTreeWidgetItem* item,int){

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
                    targetList->editItem(item,0);
                }
            }
        });

    });

    //复制
    connect(copyClassButton,&QPushButton::clicked,[=](){
        QTreeWidgetItem* item = targetList->currentItem();
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
        expand_recursive = [=,&expand_recursive](QTreeWidgetItem* parent){
            targetList->expandItem(parent);
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
    connect(clearButton,&QPushButton::clicked,[=](){
        QTreeWidgetItem* item = targetList->currentItem();
        if(item->data(0,Qt::UserRole)!=3)
            return;

        originList->addItem(item->text(1));

        item->setText(1,tr("拖拽到此处导入素材"));
        item->setTextColor(1,QColor("red"));
        item->setData(0,Qt::UserRole,2);
    });


}

//从树形Item结构中获取text(1)的文件名数组，递归，树形通用
QStringList SozaiImportDialog::getFileNamesInChildren(QTreeWidgetItem *parent)
{
    QStringList list;
    int I = parent->childCount();
    int i;


    if(parent->data(0,Qt::UserRole)==3)   //最末级子类
    {
            list << parent->text(1);
    }
    else    //被非最末级子类
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

//外部调用启动窗口
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

    /*读取配置文件中的文件列表（targetList）*/
    //加载当前配置
    QFile f(path+"/config/sozai.bkpsr");
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    doc.loadFromBinary(f.readAll());
    f.close();

    //装载列表
    loadOriginListAndTargetList_image(dir,"background",ui->originList_bg,ui->targetList_bg);
    loadOriginListAndTargetList_image(dir,"character",ui->originList_ch,ui->targetList_ch);
    loadOriginListAndTargetList_image(dir,"item",ui->originList_item,ui->targetList_item);
    loadOriginListAndTargetList_sound(dir,"bgm",ui->originList_bgm,ui->targetList_bgm);

    //启动窗口
    return this->exec();
}

void SozaiImportDialog::loadOriginListAndTargetList_image(QDir dir, const QString &folderName, QListWidget* originList, SozaiTreeWidget* targetList)
{
    //读取文件
    dir.cd("Data/"+folderName);
    QStringList imageNameFilters;
    imageNameFilters << "*.bmp" << "*.png" << "*.jpg";
    QStringList fileList = this->recursiveSearch(dir.absolutePath()+"/",dir.absolutePath()+"/", imageNameFilters);

    /*消重复、判断文件存在性、向QListView添加数据*/
    //背景图片
    QBkeDictionary targetData = doc[folderName].toBkeDic();
    std::function<void(const QBkeDictionary&, QTreeWidgetItem*)> read_recursive;
    read_recursive = [this,&read_recursive,&fileList](const QBkeDictionary &data, QTreeWidgetItem* parent){
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
                    bool exist = fileList.removeOne(member.toString());
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
                read_recursive(member.toBkeDic(),pItem);
            }
        }
    };
    read_recursive(targetData,targetList->topLevelItem(0));
    //向originList一次性添加
    originList->addItems(fileList);
    //展开树形结构
    targetList->expandAll();
}

void SozaiImportDialog::loadOriginListAndTargetList_sound(QDir dir, const QString &folderName, QListWidget* originList, SozaiTreeWidget* targetList)
{
    //读取文件
    dir.cd("Data/"+folderName);
    QStringList soundNameFilters;
    soundNameFilters << "*.ogg" << "*.wav" << "*.mp3";
    QStringList fileList = this->recursiveSearch(dir.absolutePath()+"/",dir.absolutePath()+"/", soundNameFilters);

    /*消重复、判断文件存在性、向QListView添加数据*/
    //背景图片
    QBkeDictionary targetData = doc[folderName].toBkeDic();
    std::function<void(const QBkeDictionary&, QTreeWidgetItem*)> read_recursive;
    read_recursive = [this,&read_recursive,&fileList](const QBkeDictionary &data, QTreeWidgetItem* parent){
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
                    bool exist = fileList.removeOne(member.toString());
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
                read_recursive(member.toBkeDic(),pItem);
            }
        }
    };
    read_recursive(targetData,targetList->topLevelItem(0));
    //向originList一次性添加
    originList->addItems(fileList);
    //展开树形结构
    targetList->expandAll();
}


//递归搜索某目录下全部的某特定扩展名的文件并返回数组，通用
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

//导出素材配置文件，非通用
void SozaiImportDialog::_exportSozaiConfig()
{
    doc.setVoid();  //清空
    //导出背景图片信息
    doc["background"] = _exportSozaiConfig_recursive(ui->targetList_bg->topLevelItem(0));
    doc["character"] = _exportSozaiConfig_recursive(ui->targetList_ch->topLevelItem(0));
    doc["bgm"] = _exportSozaiConfig_recursive(ui->targetList_bgm->topLevelItem(0));
}

//从当前targetList生成parser格式的存储数据，递归，树形通用
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
