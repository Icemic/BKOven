#ifndef SCRIPTGENERATOR_H
#define SCRIPTGENERATOR_H
#include <QObject>
#include "ParserHelper.h"

class ScriptGenerator: QObject
{
    Q_OBJECT
public:
    ScriptGenerator();
    ~ScriptGenerator();

    static void init();

private:
    static ScriptGenerator* self;

private slots:
    void handleQuickAdd(const QString &type, const QString &name, const QString &filePath, const QBkeVariable &data);
};

#endif // SCRIPTGENERATOR_H
