#ifndef SCRIPTLISTWIDGET_H
#define SCRIPTLISTWIDGET_H

#include <QListWidget>
#include <QMenu>
#include "ScriptListWidgetItem.h"
#include "ParserHelper.h"

class ScriptListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit ScriptListWidget(QWidget* parent=0);
    ~ScriptListWidget();

    void addScript(const QString &type, const QBkeVariable &data);

    ScriptListWidgetItem* script(int row);
    void freshDescription(int row);

    QBkeVariable scriptData();
    void setScriptData(const QBkeArray &arr);
    void setUuid(const QString &uuid);
    QString uuid();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *) override;

private:
    void _addScript(const QString &type, const QBkeVariable &data);
    QString _uuid;
    QMenu* rMenu;
    QList<QAction*> itemMenuActions;

signals:
    void scriptClicked(const QString &type, QBkeVariableRef data, int row);
    void scriptDoubleClicked(const QString &type, QBkeVariableRef data, int row);

    void scriptAdded(int row);
    void scriptDeleted(int start, int count);
};

#endif // SCRIPTLISTWIDGET_H
