#include "SozaiWidgetPrivate.h"
#include "ui_SozaiWidgetPrivate.h"
#include <QFile>
#include <QDebug>

SozaiWidgetPrivate::SozaiWidgetPrivate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SozaiWidgetPrivate)
{
    ui->setupUi(this);

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
}
