#include "GameConfig.h"
#include "ui_GameConfig.h"
#include <QFileDialog>
#include <QMessageBox>

GameConfig::GameConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameConfig)
{
    ui->setupUi(this);
}

GameConfig::~GameConfig()
{
    delete ui;
}

void GameConfig::execWithPath(const QString &projectPath)
{
    this->projectPath = projectPath;

    QFile actAndSceneFile(projectPath+"/config/ActAndScene.bkpsr");
    actAndSceneFile.open(QIODevice::ReadOnly);
    actAndSceneDoc.loadFromBinary(actAndSceneFile.readAll());
    actAndSceneFile.close();

    initTitleTab();

    exec();
}

void GameConfig::initTitleTab()
{
    fillComboBoxWithScene(ui->title_entranceComboBox);
    fillComboBoxWithScene(ui->title_firstEnterComboBox);

    auto bindFileOpenWithLineEdit = [=](QPushButton* button,QLineEdit* lineedit){
        lineedit->setDisabled(true);
        connect(button,&QPushButton::clicked,[=](){
            for(;;)
            {
                QString filename = QFileDialog::getOpenFileName(this,tr("选择素材"),projectPath+"/Data/ui");
                if(filename.isEmpty()) break;
                if(filename.indexOf(projectPath+"/Data/ui")==-1)
                {
                    QMessageBox::warning(this,tr("选择素材"),tr("所选文件必须在 %1 目录下").arg(projectPath+"/Data/ui"),QMessageBox::Ok,QMessageBox::NoButton);
                }
                else
                {
                    lineedit->setText(filename.replace(projectPath+"/Data/ui/",""));
                    break;
                }
            }
        });
    };

    bindFileOpenWithLineEdit(ui->title_button_bgm,ui->title_lineEdit_bgm);
    bindFileOpenWithLineEdit(ui->title_button_touchSe,ui->title_lineEdit_touchSe);
    bindFileOpenWithLineEdit(ui->title_button_hoverSe,ui->title_lineEdit_hoverSe);
    bindFileOpenWithLineEdit(ui->title_button_start,ui->title_lineEdit_start);
    bindFileOpenWithLineEdit(ui->title_button_load,ui->title_lineEdit_load);
    bindFileOpenWithLineEdit(ui->title_button_config,ui->title_lineEdit_config);
    bindFileOpenWithLineEdit(ui->title_button_exit,ui->title_lineEdit_exit);
    bindFileOpenWithLineEdit(ui->title_button_cg,ui->title_lineEdit_cg);
    bindFileOpenWithLineEdit(ui->title_button_music,ui->title_lineEdit_music);
    bindFileOpenWithLineEdit(ui->title_button_about,ui->title_lineEdit_about);


}

void GameConfig::loadDataOfTitleTab()
{

}

void GameConfig::fillComboBoxWithScene(QComboBox *comboBox)
{
    if(actAndSceneDoc.isNull())
    {
        comboBox->addItem(tr("无"));
        return;
    }

    QBkeDictionary dic = actAndSceneDoc.toBkeDic();
    QBkeDictionary::const_iterator ite;
    for(ite=dic.cbegin();ite!=dic.cend();ite++)
    {
        QString parent = ite.key();
        QBkeDictionary childDic = ite.value().toBkeDic();
        QBkeDictionary::const_iterator childIte;
        for(childIte=childDic.cbegin();childIte!=childDic.end();childIte++)
        {
            comboBox->addItem(parent+"/"+childIte.key(),childIte.value().toString());
        }
    }
    comboBox->setCurrentIndex(0);
}
