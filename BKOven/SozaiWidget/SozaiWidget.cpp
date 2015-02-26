#include "SozaiWidget.h"
#include <QDebug>

SozaiWidgetPrivate* SozaiWidget::_widget = nullptr;
SozaiWidget::SozaiWidget()
{
}

void SozaiWidget::test()
{
    SozaiWidget::init();
    _widget->show();
    qDebug() << "SozaiWidget: oka!" << endl;

}

void SozaiWidget::init()
{
    _widget = new SozaiWidgetPrivate();

}

void SozaiWidget::open(const QString &projectPath)
{
    _widget->open(projectPath);

}

SozaiWidgetPrivate* SozaiWidget::widget()
{
    return _widget;
}
