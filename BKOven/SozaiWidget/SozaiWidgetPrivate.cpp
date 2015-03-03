#include "SozaiWidgetPrivate.h"
#include "ui_SozaiWidgetPrivate.h"
#include "ImageSelectDialog.h"
#include "SoundSelectDialog.h"
#include "BackgroundAddDialog.h"
#include "CharacterAddDialog.h"
#include "ItemAddDialog.h"
#include "SoundAddDialog.h"
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
    QBkeVariable data = QBkeVariable::dic();
    data.insert("fade",true);
    data.insert("from",0);
    data.insert("to",255);
    data.insert("msec",800);
    if(type=="character")
    {
        data.insert("freepos",false);
        data.insert("pos",4);
    }
    emit fileAdd(type,name,filePath,data);
}
void SozaiWidgetPrivate::on_add(const QString &type, const QString &name, const QString &filePath)
{
    bool fade,freepos;
    int from,to,msec,pos;
    QBkeVariable data = QBkeVariable::dic();
    if(type=="background")
    {
        BackgroundAddDialog addDialog;
        if(addDialog.exec(fade,from,to,msec))
        {
            data.insert("fade",fade);
            data.insert("from",from);
            data.insert("to",to);
            data.insert("msec",msec);
            emit fileAdd(type,name,filePath,data);
        }
    }
    else if(type=="character")
    {
        CharacterAddDialog addDialog;
        if(addDialog.exec(freepos,pos,fade,from,to,msec))
        {
            data.insert("freepos",freepos);
            data.insert("pos",pos);
            data.insert("fade",fade);
            data.insert("from",from);
            data.insert("to",to);
            data.insert("msec",msec);
            emit fileAdd(type,name,filePath,data);
        }
    }
    else if(type=="item")
    {
        ItemAddDialog addDialog;
        if(addDialog.exec(fade,from,to,msec))
        {
            data.insert("fade",fade);
            data.insert("from",from);
            data.insert("to",to);
            data.insert("msec",msec);
            emit fileAdd(type,name,filePath,data);
        }
    }
    else if(type=="bgm" || type=="se")
    {
        SoundAddDialog addDialog;
        if(addDialog.exec(fade,from,to,msec))
        {
            data.insert("fade",fade);
            data.insert("from",from);
            data.insert("to",to);
            data.insert("msec",msec);
            emit fileAdd(type,name,filePath,data);
        }
    }
    else
        qWarning() << "SozaiWidgetPrivate: 未知的类型 "+type << endl;
}
