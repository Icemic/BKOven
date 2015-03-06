#include "PreviewWidget.h"
#include <QOpenGLWidget>
#include <QDebug>

PreviewWidget::PreviewWidget(int w, int h, const QString &projectPath,QWidget* parent):
    QGraphicsView(parent)
{
//    this->set
    this->w = w;
    this->h = h;
    this->projectPath = projectPath;
    scene = new PreviewScene();
    scene->setSceneRect(0,0,w,h);
    this->setScene(scene);
    QOpenGLWidget *glWidget = new QOpenGLWidget();
    this->setViewport(glWidget);


    backgroundItem = new QGraphicsPixmapItem();
    backgroundItem->setZValue(0);

    textWindowItem = new QGraphicsPixmapItem();
    textWindowItem->setZValue(2000);
    textWindowTextItem = new QGraphicsTextItem();
    textWindowTextItem->setFlag(QGraphicsItem::ItemIsMovable,true);
    textWindowTextItem->setFlag(QGraphicsItem::ItemIsSelectable,true);
    textWindowTextItem->setData(0,"textwindowtext");
    textWindowTextItem->setZValue(2);
    textWindowTextItem->setParentItem(textWindowItem);
    nameBoxTextItem = new QGraphicsTextItem();
    nameBoxTextItem->setFlag(QGraphicsItem::ItemIsMovable,true);
    nameBoxTextItem->setFlag(QGraphicsItem::ItemIsSelectable,true);
    nameBoxTextItem->setData(0,"textwindowtext");
    nameBoxTextItem->setZValue(2);
    nameBoxTextItem->setParentItem(textWindowItem);

    connect(scene,&PreviewScene::itemMoved,[=](const QString &type, const QString &filename, int x, int y){
        emit itemMoved(type, filename, x, y);
    });

}

PreviewWidget::~PreviewWidget()
{

}

void PreviewWidget::setBackgroundImage(const QString &filename)
{
    backgroundItem->setPixmap(QPixmap(projectPath+"/Data/"+filename));
    if(backgroundItem->scene()==nullptr)
        scene->addItem(backgroundItem);
    backgroundItem->setData(0,"background");
    backgroundItem->setData(1,filename);
}

void PreviewWidget::setCharacterImage(const QString &filename, int position)
{
    if(!characterItems.contains(filename)){
        QGraphicsPixmapItem* item = new QGraphicsPixmapItem;
        characterItems.insert(filename,item);
    }
    QGraphicsPixmapItem* item = characterItems.value(filename);
    if(item->scene()==nullptr)
        scene->addItem(item);
    item->setFlag(QGraphicsItem::ItemIsMovable,false);
    item->setFlag(QGraphicsItem::ItemIsSelectable,true);
    item->setData(0,"character");
    item->setData(1,filename);
    item->setPixmap(QPixmap(projectPath+"/Data/"+filename));
    item->setOffset(-item->pixmap().width()/2,-item->pixmap().height());
    item->setPos((this->w/8.)*position,scene->height());
}

void PreviewWidget::setCharacterImage(const QString &filename, int x, int y)
{
    if(!characterItems.contains(filename)){
        characterItems.insert(filename,new QGraphicsPixmapItem);
    }
    QGraphicsPixmapItem* item = characterItems.value(filename);
    if(item->scene()==nullptr)
        scene->addItem(item);
    item->setFlag(QGraphicsItem::ItemIsMovable,true);
    item->setFlag(QGraphicsItem::ItemIsSelectable,true);
    item->setData(0,"character");
    item->setData(1,filename);
    item->setPixmap(QPixmap(projectPath+"/Data/"+filename));
    item->setPos(x,y);
}

void PreviewWidget::setItemImage(const QString &filename, int x, int y)
{
    if(!itemItems.contains(filename)){
        itemItems.insert(filename,new QGraphicsPixmapItem);
    }
    QGraphicsPixmapItem* item = itemItems.value(filename);
    if(item->scene()==nullptr)
        scene->addItem(item);
    item->setFlag(QGraphicsItem::ItemIsMovable,true);
    item->setFlag(QGraphicsItem::ItemIsSelectable,true);
    item->setData(0,"item");
    item->setData(1,filename);
    item->setPixmap(QPixmap(projectPath+"/Data/"+filename));
    item->setPos(x,y);
}

void PreviewWidget::removeBackgroundImage()
{
    scene->removeItem(backgroundItem);
}

void PreviewWidget::removeCharacterImage(const QString &filename)
{
    if(characterItems.contains(filename))
    {
        QGraphicsPixmapItem* item = characterItems.value(filename);
        scene->removeItem(item);
    }
}

void PreviewWidget::removeItemImage(const QString &filename)
{
    if(itemItems.contains(filename))
    {
        QGraphicsPixmapItem* item = itemItems.value(filename);
        scene->removeItem(item);
    }
}

void PreviewWidget::setTextWindowImage(const QString &filename, int x, int y)
{
    textWindowItem->setPixmap(QPixmap(projectPath+"/Data/"+filename));
    textWindowItem->setPos(x,y);
    if(textWindowItem->scene()==nullptr)
        scene->addItem(textWindowItem);
    textWindowItem->setFlag(QGraphicsItem::ItemIsMovable,true);
    textWindowItem->setFlag(QGraphicsItem::ItemIsSelectable,true);
    textWindowItem->setData(0,"textwindow");
    textWindowItem->setData(1,filename);
}

void PreviewWidget::setTextWindowTextPos(int rx, int ry)
{
    textWindowTextItem->setPos(rx,ry);
}

void PreviewWidget::setNameBoxTextPos(int rx, int ry)
{
    nameBoxTextItem->setPos(rx,ry);
}

void PreviewWidget::setTextWindowTextStyle(int size, int maxWidth, int maxHeight, const QString &color, const QString &align,
                                           bool bold, bool underlined, bool italic,
                                           bool stroke, const QString &strokeColor, bool shadow, const QString &shadowColor)
{
    textWindowTextItem;
    textWindowTextItem->setTextWidth(0);
}
