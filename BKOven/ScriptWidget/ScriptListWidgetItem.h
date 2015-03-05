#ifndef SCRIPTLISTWIDGETITEM_H
#define SCRIPTLISTWIDGETITEM_H

#include <QListWidgetItem>
#include "ParserHelper.h"

class ScriptListWidgetItem : public QListWidgetItem
{
public:
    explicit ScriptListWidgetItem();
    ~ScriptListWidgetItem();

    QString type();
    QString description();
    QBkeVariable scriptData();
    QBkeVariableRef scriptDataRef();

    void setScriptData(const QBkeVariable &data);
    void setType(const QString &type);
    void setDescription(const QString &text);

private:
    QString _type;
    QString _description;
    QBkeVariable _scriptData;
};

#endif // SCRIPTLISTWIDGETITEM_H
