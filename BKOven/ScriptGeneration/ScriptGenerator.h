#ifndef SCRIPTGENERATOR_H
#define SCRIPTGENERATOR_H
#include "SozaiWidget/SozaiWidget.h"
#include "StageWidget/StageWidget.h"
#include "StageWidget/StageWidget.h"

class ScriptGenerator: QObject
{
    Q_OBJECT
public:
    ScriptGenerator();
    ~ScriptGenerator();

private slots:
    void handleQuickAdd(const QString &type, const QString &name, const QString &filePath, bool fade, int from, int to, int msec);
};

#endif // SCRIPTGENERATOR_H
