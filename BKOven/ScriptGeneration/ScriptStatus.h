#ifndef SCRIPTSTATUS_H
#define SCRIPTSTATUS_H
#include "../ScriptWidget/ScriptListWidget.h"
#include "ParserHelper.h"

class ScriptStatus
{
public:
    ScriptStatus();
    ~ScriptStatus();

    static void init();
    static ScriptStatus* self();

    void open(const QString &projectPath, const QString &uuid);
    void save();
    void analysis(ScriptListWidget* widget);
    void analysisOne(ScriptListWidget *widget, int row);
    void dropFrom(int row);
    void dropAfter(int row);

    QBkeVariable currentStatus(int row);

private:
    void _analysis(QBkeVariable &item, const QBkeVariable &data);

    static ScriptStatus* _self;
    QString projectPath;
    QString uuid;

    QMap<int,QString> characterFixedList;
    QBkeVariable doc;
};

#endif // SCRIPTSTATUS_H
