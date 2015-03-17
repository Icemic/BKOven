#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include "previewwidget_global.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QResizeEvent>
#include "PreviewScene.h"
#include "../BKFontText/BKFontText.h"

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
        foreach(QGraphicsItem* item, scene->items())
        {
            scene->removeItem(item);
        }
    }

    void removeBackgroundImage();
    void removeCharacterImage(int position);
    void removeCharacterImage(const QString &filename);
    void removeItemImage(const QString &filename);

    void setTextWindowImage(const QString &filename, int x, int y);
    void setTextWindowTextPos(int rx, int ry);
    void setNameBoxTextPos(int rx, int ry);

    void renderTextWindowText(const QString &text, int w=-1, int h=-1);
    void renderNameBoxText(const QString &text, int w=-1, int h=-1);

    void setTextWindowTextFontInfo(const QString &name, const QColor &color=QColor("white"), const bklong &style=bkeStyleNormal, int size=24)
    {
        BKFontText::getInstance()->setFontInfo("PreviewWidget_textwindowtext",projectPath+"/Data/"+name,color,style,size);
    }

    void setTextWindowTextBold(bool state)
    {
        BKFontText::getInstance()->setBold("PreviewWidget_textwindowtext",state);
    }

    void setTextWindowTextItalic(bool state){
        BKFontText::getInstance()->setItalic("PreviewWidget_textwindowtext",state);
    }

    void setTextWindowTextUnderlined(bool state)
    {
        BKFontText::getInstance()->setUnderlined("PreviewWidget_textwindowtext",state);
    }

    void setTextWindowTextStrike(bool state)
    {
        BKFontText::getInstance()->setStrike("PreviewWidget_textwindowtext",state);
    }

    void setTextWindowTextShadow(bool state, const QColor &color=QColor())
    {
        BKFontText::getInstance()->setShadow("PreviewWidget_textwindowtext",state,color);
    }

    void setTextWindowTextStroke(bool state, const QColor &color=QColor())
    {
        BKFontText::getInstance()->setStroke("PreviewWidget_textwindowtext",state,color);
    }

    void setNameBoxTextFontInfo(const QString &name, const QColor &color=QColor("white"), const bklong &style=bkeStyleNormal, int size=24)
    {
        BKFontText::getInstance()->setFontInfo("PreviewWidget_nameBoxtext",projectPath+"/Data/"+name,color,style,size);
    }

    void setNameBoxTextBold(bool state)
    {
        BKFontText::getInstance()->setBold("PreviewWidget_nameBoxtext",state);
    }

    void setNameBoxTextItalic(bool state)
    {
        BKFontText::getInstance()->setItalic("PreviewWidget_nameBoxtext",state);
    }

    void setNameBoxTextUnderlined(bool state)
    {
        BKFontText::getInstance()->setUnderlined("PreviewWidget_nameBoxtext",state);
    }

    void setNameBoxTextStrike(bool state)
    {
        BKFontText::getInstance()->setStrike("PreviewWidget_nameBoxtext",state);
    }

    void setNameBoxTextShadow(bool state, const QColor &color=QColor())
    {
        BKFontText::getInstance()->setShadow("PreviewWidget_nameBoxtext",state,color);
    }

    void setNameBoxTextStroke(bool state, const QColor &color=QColor())
    {
        BKFontText::getInstance()->setStroke("PreviewWidget_nameBoxtext",state,color);
    }

protected:
    virtual void resizeEvent(QResizeEvent *e) override;

private:
    int w;
    int h;
    QString projectPath;

    PreviewScene* scene;
    QGraphicsPixmapItem* backgroundItem;
    QMap<QString,QGraphicsPixmapItem*> characterItems;
    QMap<QString,QGraphicsPixmapItem*> itemItems;

    QGraphicsPixmapItem* textWindowItem;
    QGraphicsPixmapItem* nameBoxTextItem;
    QGraphicsPixmapItem* textWindowTextItem;

    QMap<QString,QGraphicsPixmapItem*> textWindowButtonItems;

signals:
    void itemMoved(const QString &type, const QString &filename, int x, int y);

};

#endif // PREVIEWWIDGET_H
