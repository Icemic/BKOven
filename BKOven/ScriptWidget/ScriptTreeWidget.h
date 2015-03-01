#ifndef SCRIPTTREEWIDGET_H
#define SCRIPTTREEWIDGET_H

#include <QTreeWidget>
#include <QMenu>
#include "ParserHelper.h"

class ScriptTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit ScriptTreeWidget(QWidget* parent=0);
    ~ScriptTreeWidget();

    QBkeVariable scriptData();
    void setScriptData(const QBkeDictionary &dic);

protected:
    virtual void contextMenuEvent(QContextMenuEvent *) override;
private:
    QMenu* rMenu;
    QList<QAction*> blankMenuActions;
    QList<QAction*> parentMenuActions;
    QList<QAction*> childMenuActions;

signals:
    void requestShowScripts(const QString &uuid);
};

#endif // SCRIPTTREEWIDGET_H
