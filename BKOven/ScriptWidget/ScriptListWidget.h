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

    QBkeVariable scriptData();
    void setScriptData(const QBkeArray &arr);
    void setUuid(const QString &uuid);
    QString uuid();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *) override;

private:
    QString _uuid;
    QMenu* rMenu;
    QListWidgetItem* insertPoint = nullptr;
    QList<QAction*> itemMenuActions;

signals:
    void scriptClicked(const QString &type, const QString &description, QBkeVariableRef data);
    void scriptDoubleClicked(const QString &type, const QString &description, QBkeVariableRef data);
};

#endif // SCRIPTLISTWIDGET_H
