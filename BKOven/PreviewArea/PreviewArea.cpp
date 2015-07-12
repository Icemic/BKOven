#include "PreviewArea.h"
#include <QDebug>

PreviewWidget* PreviewArea::_widget = nullptr;
QString PreviewArea::projectPath;
PreviewArea::PreviewArea()
{

}

PreviewArea::~PreviewArea()
{

}

void PreviewArea::test()
{
    qDebug() << "PreviewArea: ok!" << endl;
}

void PreviewArea::init()
{
    if(_widget==nullptr)
    {
        _widget = new PreviewWidget(800,600);
    }
    _widget->clear();
}

void PreviewArea::open(const QString &projectPath, int w, int h)
{
    PreviewArea::projectPath = projectPath;
    if(_widget==nullptr)
    {
        _widget = new PreviewWidget(w,h,projectPath);
    }
    _widget->setSize(w,h);
    _widget->setProjectPath(projectPath);
    _widget->clear();
}

PreviewWidget* PreviewArea::widget()
{
    return _widget;
}

void PreviewArea::update(const QBkeVariable &doc)
{
    _widget->clear();
    QBkeVariable bg = doc["background"];
    if(!bg.isNull())
        _widget->setBackgroundImage(bg["type"].toString()+"/"+bg["filePath"].toString());
    QBkeDictionary &characterDic = doc["character"].toBkeDic();
    foreach(QBkeVariable item, characterDic)
    {
        if(item["freepos"].toBool())
        {
            _widget->setCharacterImage(item["type"].toString()+"/"+item["filePath"].toString(),item["x"].toInt(),item["y"].toInt());
        }
        else
        {
            _widget->setCharacterImage(item["type"].toString()+"/"+item["filePath"].toString(),item["pos"].toInt());
        }
    }
    QBkeDictionary &itemDic = doc["item"].toBkeDic();
    foreach(QBkeVariable item, itemDic)
    {
        _widget->setItemImage(item["type"].toString()+"/"+item["filePath"].toString(),item["x"].toInt(),item["y"].toInt());
    }
}
