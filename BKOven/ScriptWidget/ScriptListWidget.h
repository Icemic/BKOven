#ifndef SCRIPTLISTWIDGET_H
#define SCRIPTLISTWIDGET_H

#include <QListWidget>
#include "ScriptListWidgetItem.h"
#include "ParserHelper.h"

class ScriptListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit ScriptListWidget(QWidget* parent=0);
    ~ScriptListWidget();

    void addScript(const QString &type, const QString &description, const QBkeVariable &data);

    ScriptListWidgetItem* script(int row);

    QBkeVariable scriptData();
    void setScriptData(const QBkeArray &arr);
    void setUuid(const QString &uuid);
    QString uuid();

private:
    QString _uuid;

signals:
    void scriptClicked(const QString &type, const QString &description, QBkeVariableRef data);
    void scriptDoubleClicked(const QString &type, const QString &description, QBkeVariableRef data);
};

#endif // SCRIPTLISTWIDGET_H
