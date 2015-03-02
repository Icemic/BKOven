#include "SozaiWidgetPrivate.h"
#include "ui_SozaiWidgetPrivate.h"
#include "ImageSelectDialog.h"
#include "SoundSelectDialog.h"
#include "AddConfigDialog.h"
#include <QFile>
#include <QDebug>

SozaiWidgetPrivate::SozaiWidgetPrivate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SozaiWidgetPrivate)
{
    ui->setupUi(this);
    ui->stackedWidget->setContentsMargins(0,0,0,0);

    connect(ui->bgButton,&QPushButton::clicked,[=](){
        ui->stackedWidget->setCurrentIndex(0);
    });
    connect(ui->chButton,&QPushButton::clicked,[=](){
        ui->stackedWidget->setCurrentIndex(1);
    });
    connect(ui->itemButton,&QPushButton::clicked,[=](){
        ui->stackedWidget->setCurrentIndex(2);
    });
    connect(ui->bgmButton,&QPushButton::clicked,[=](){
        ui->stackedWidget->setCurrentIndex(3);
    });
    connect(ui->seButton,&QPushButton::clicked,[=](){
        ui->stackedWidget->setCurrentIndex(4);
    });

    setupConnections();
}

SozaiWidgetPrivate::~SozaiWidgetPrivate()
{
    delete ui;
}

void SozaiWidgetPrivate::open(const QString &path)
{
    QFile f(path+"/config/sozai.bkpsr");
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    doc.loadFromBinary(f.readAll());
    f.close();

    ui->bgImageSelectWidget->load(path,"background",doc["background"].toBkeDic());
    ui->chImageSelectWidget->load(path,"character",doc["character"].toBkeDic());
    ui->itemImageSelectWidget->load(path,"item",doc["item"].toBkeDic());

    ui->bgmSoundSelectWidget->load(path,"bgm",doc["bgm"].toBkeDic());
    ui->seSoundSelectWidget->load(path,"se",doc["se"].toBkeDic());
}

void SozaiWidgetPrivate::setupConnections()
{
    connect(ui->bgImageSelectWidget,SIGNAL(quickAdd(QString,QString,QString)),this,SLOT(on_quickAdd(QString,QString,QString)));
    connect(ui->bgImageSelectWidget,SIGNAL(add(QString,QString,QString)),this,SLOT(on_add(QString,QString,QString)));
    connect(ui->chImageSelectWidget,SIGNAL(quickAdd(QString,QString,QString)),this,SLOT(on_quickAdd(QString,QString,QString)));
    connect(ui->chImageSelectWidget,SIGNAL(add(QString,QString,QString)),this,SLOT(on_add(QString,QString,QString)));
    connect(ui->itemImageSelectWidget,SIGNAL(quickAdd(QString,QString,QString)),this,SLOT(on_quickAdd(QString,QString,QString)));
    connect(ui->itemImageSelectWidget,SIGNAL(add(QString,QString,QString)),this,SLOT(on_add(QString,QString,QString)));
    connect(ui->bgmSoundSelectWidget,SIGNAL(quickAdd(QString,QString,QString)),this,SLOT(on_quickAdd(QString,QString,QString)));
    connect(ui->bgmSoundSelectWidget,SIGNAL(add(QString,QString,QString)),this,SLOT(on_add(QString,QString,QString)));
    connect(ui->seSoundSelectWidget,SIGNAL(quickAdd(QString,QString,QString)),this,SLOT(on_quickAdd(QString,QString,QString)));
    connect(ui->seSoundSelectWidget,SIGNAL(add(QString,QString,QString)),this,SLOT(on_add(QString,QString,QString)));
}

void SozaiWidgetPrivate::on_quickAdd(const QString &type, const QString &name, const QString &filePath)
{
    //快速添加的默认设置，淡入0-255 800ms
    emit fileAdd(type,name,filePath,true);
}
void SozaiWidgetPrivate::on_add(const QString &type, const QString &name, const QString &filePath)
{
    bool fade;
    int from,to,msec;
    AddConfigDialog addDialog;
    if(addDialog.exec(fade,from,to,msec)){
        emit fileAdd(type,name,filePath,fade,from,to,msec);
    }
}
