#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include "previewwidget_global.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include "PreviewScene.h"

class PREVIEWWIDGETSHARED_EXPORT PreviewWidget: public QGraphicsView
{
    Q_OBJECT
public:
    explicit PreviewWidget(int w, int h, const QString &projectPath, QWidget *parent=0);
    ~PreviewWidget();

    void setBackgroundImage(const QString &filename);
    void setCharacterImage(const QString &filename, int position);
    void setCharacterImage(const QString &filename, int x, int y);
    void setItemImage(const QString &filename, int x, int y);

    void clear(){
        scene->clear();
    }

    void removeBackgroundImage();
    void removeCharacterImage(int position);
    void removeCharacterImage(const QString &filename);
    void removeItemImage(const QString &filename);

    void setTextWindowImage(const QString &filename, int x, int y);
    void setTextWindowTextPos(int rx, int ry);
    void setNameBoxTextPos(int rx, int ry);
    void setTextWindowTextStyle(int size, int maxWidth, int maxHeight, const QString &color, const QString &align,
                                bool bold, bool underlined, bool italic, bool stroke, const QString &strokeColor, bool shadow, const QString &shadowColor);
    void setNameBoxTextStyle(int size, int maxWidth, QString &color, const QString &align,
                             bool bold, bool underlined, bool italic, bool stroke, const QString &strokeColor, bool shadow, const QString &shadowColor);


private:
    int w;
    int h;
    QString projectPath;

    PreviewScene* scene;
    QGraphicsPixmapItem* backgroundItem;
    QMap<QString,QGraphicsPixmapItem*> characterItems;
    QMap<QString,QGraphicsPixmapItem*> itemItems;

    QGraphicsPixmapItem* textWindowItem;
    QGraphicsTextItem* nameBoxTextItem;
    QGraphicsTextItem* textWindowTextItem;

    QMap<QString,QGraphicsPixmapItem*> textWindowButtonItems;

signals:
    void itemMoved(const QString &type, const QString &filename, int x, int y);

};

#endif // PREVIEWWIDGET_H
