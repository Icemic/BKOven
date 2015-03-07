#include "PreviewScene.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QDebug>

PreviewScene::PreviewScene()
{
}

PreviewScene::~PreviewScene()
{

}

void PreviewScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    QList<QGraphicsItem*> items = selectedItems();
    foreach(QGraphicsItem* item, items)
    {
        emit itemMoved(item->data(0).toString(), item->data(1).toString(), item->x(), item->y());
    }
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}
