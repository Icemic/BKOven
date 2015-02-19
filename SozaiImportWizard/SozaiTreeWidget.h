#ifndef SOZAITREEWIDGET_H
#define SOZAITREEWIDGET_H

#include <QTreeWidget>

class SozaiTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit SozaiTreeWidget(QWidget *parent = 0);
    ~SozaiTreeWidget();

signals:
    void dropped(QTreeWidgetItem* target, const QMap<int,QVariant> & source );

private:


protected:
    virtual bool dropMimeData ( QTreeWidgetItem * parent, int index, const QMimeData * data, Qt::DropAction action ) override;
    void dropEvent(QDropEvent *event);
};

#endif // SOZAITREEWIDGET_H
