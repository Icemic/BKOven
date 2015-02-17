#include "SozaiTreeWidget.h"
#include <QDropEvent>
#include <QMimeData>
#include <QMessageBox>
#include <QListWidget>
#include <QDebug>

SozaiTreeWidget::SozaiTreeWidget(QWidget *parent):
    QTreeWidget(parent)
{

}

SozaiTreeWidget::~SozaiTreeWidget()
{

}

bool SozaiTreeWidget::dropMimeData( QTreeWidgetItem * parent, int index, const QMimeData * data, Qt::DropAction action ){
    if(parent == nullptr)
        return false;
    if(data->hasFormat("application/x-qabstractitemmodeldatalist")){
        QByteArray encoded = data->data("application/x-qabstractitemmodeldatalist");
        QDataStream stream(&encoded, QIODevice::ReadOnly);

        while (!stream.atEnd())
        {
            int row, col;
            QMap<int,  QVariant> roleDataMap;
            stream >> row >> col >> roleDataMap;

            emit dropped(parent,roleDataMap);
//            parent->setText(1,roleDataMap.value(0).toString());
//            parent->setTextColor(1,QColor("black"));

            return true;
        }
    }
    //QTreeWidget::dropMimeData(parent, index, data, action);
    return false;
}

void SozaiTreeWidget::dropEvent(QDropEvent *event)
{
    QTreeWidget::dropEvent(event);

    //if(event->source()->objectName()=="")
    //dropTarget->setText(1,static_cast<QListWidget*>(event->source())->currentItem()->text());
    //QMessageBox::information(this, "", event->source()->objectName());

}
