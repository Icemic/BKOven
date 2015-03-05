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
