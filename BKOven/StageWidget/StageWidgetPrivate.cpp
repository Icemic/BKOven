#include "StageWidgetPrivate.h"
#include "ui_StageWidgetPrivate.h"

StageWidgetPrivate::StageWidgetPrivate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StageWidgetPrivate)
{
    ui->setupUi(this);
}

StageWidgetPrivate::~StageWidgetPrivate()
{
    delete ui;
}

void StageWidgetPrivate::setStageStatus(const QBkeVariable &data)
{
    ui->backgroundListWidget->clear();
    ui->characterListWidget->clear();
    ui->itemListWidget->clear();
    ui->soundListWidget->clear();
    ui->backgroundListWidget->addItem(data["background"]["name"].toString());
    ui->characterListWidget->addItems(data["character"].getKeys());
    ui->itemListWidget->addItems(data["item"].getKeys());
    ui->soundListWidget->addItem(data["bgm"]["name"].toString());
    ui->soundListWidget->addItems(data["se"].getKeys());
}
