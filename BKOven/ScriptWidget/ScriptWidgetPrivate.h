﻿#ifndef SCRIPTWIDGETPRIVATE_H
#define SCRIPTWIDGETPRIVATE_H

#include <QWidget>
#include "ScriptListWidgetItem.h"
#include "ParserHelper.h"

namespace Ui {
class ScriptWidgetPrivate;
}

class ScriptWidgetPrivate : public QWidget
{
    Q_OBJECT

public:
    explicit ScriptWidgetPrivate(QWidget *parent = 0);
    ~ScriptWidgetPrivate();

    void open(const QString &path);
    void save();

    bool addScript(const QString &type, const QBkeVariable &data);


private:
    Ui::ScriptWidgetPrivate *ui;

    QString projectPath;

    QBkeDictionary sceneScripts;

    void setupConnections();
};

#endif // SCRIPTWIDGETPRIVATE_H
