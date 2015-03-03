#ifndef DESCRIPTIONGENERATOR_H
#define DESCRIPTIONGENERATOR_H
#include <QtCore>
#include <QObject>
#include "ParserHelper.h"

class DescriptionGenerator:QObject
{
public:
    DescriptionGenerator();
    ~DescriptionGenerator();

    static QString description(const QString &type, const QBkeVariable &value);

private:
    static QString characterPosition(int index);
};

#endif // DESCRIPTIONGENERATOR_H
