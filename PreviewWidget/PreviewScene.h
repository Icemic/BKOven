#ifndef PREVIEWSCENE_H
#define PREVIEWSCENE_H

#include <QGraphicsScene>

class PreviewScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit PreviewScene();
    ~PreviewScene();

protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent) override;

signals:
    void itemMoved(const QString &type, const QString &filename, int x, int y);
};

#endif // PREVIEWSCENE_H
