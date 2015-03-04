#include "StageWidget.h"
#include <QDebug>

StageWidgetPrivate* StageWidget::_widget = nullptr;
StageWidget::StageWidget()
{

}

StageWidget::~StageWidget()
{

}

void StageWidget::test()
{
    qDebug() << "StageWidget: ok!" << endl;
}

void StageWidget::init()
{
    _widget = new StageWidgetPrivate();
}

StageWidgetPrivate* StageWidget::widget()
{
    return _widget;
}

void StageWidget::setStageStatus(const QBkeVariable &data)
{
    _widget->setStageStatus(data);
}
