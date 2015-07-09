#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <QDialog>
#include <QComboBox>
#include "../ParserHelper/ParserHelper.h"

namespace Ui {
class GameConfig;
}

class GameConfig : public QDialog
{
    Q_OBJECT

public:
    explicit GameConfig(QWidget *parent = 0);
    ~GameConfig();

    void execWithPath(const QString &projectPath);

private:
    Ui::GameConfig *ui;
    QString projectPath;
    QBkeVariable actAndSceneDoc;

    void fillComboBoxWithScene(QComboBox* comboBox);

    void initBaseTab();
    void saveDataOfBaseTab();
    void loadDataOfBaseTab();
    void initTitleTab();
    void saveDataOfTitleTab();
    void loadDataOfTitleTab();
};

#endif // GAMECONFIG_H
