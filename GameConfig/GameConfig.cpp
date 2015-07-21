#include "GameConfig.h"
#include "ui_GameConfig.h"
#include <QFileDialog>
#include <QMessageBox>

GameConfig::GameConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameConfig)
{
    ui->setupUi(this);

    connect(this,&QDialog::accepted,[=](){
        QDir dir(projectPath);
        dir.mkpath("config/ui");
        saveDataOfBaseTab();
    });


    //test



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

    initBaseTab();
    initTitleTab();

    loadDataOfBaseTab();
    loadDataOfTitleTab();

    exec();
}

//基本设置选项卡
void GameConfig::initBaseTab()
{
    fillComboBoxWithScene(ui->base_comboBox_entrance);
}

void GameConfig::saveDataOfBaseTab()
{
    QBkeVariable doc;
    doc.insert("game",ui->base_lineEdit_game->text());
    doc.insert("author",ui->base_lineEdit_author->text());
    doc.insert("staff",ui->base_plainTextEdit_staff->toPlainText());
    doc.insert("copyright",ui->base_plainTextEdit_copyright->toPlainText());
    doc.insert("entrance",ui->base_comboBox_entrance->currentText());    //禁止/字符
    doc.insert("mode",ui->base_radioButton_window->isChecked()?"window":"fullscreen");

    QFile file(projectPath+"/config/ui/base.bkpsr");
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    file.write(doc.saveToBinary(true));
    file.close();

}

void GameConfig::loadDataOfBaseTab()
{
    QBkeVariable doc;
    QFile file(projectPath+"/config/ui/base.bkpsr");
    file.open(QIODevice::ReadOnly);
    doc.loadFromBinary(file.readAll());
    file.close();

    ui->base_lineEdit_game->setText(doc.value("game").toString());
    ui->base_lineEdit_author->setText(doc.value("author").toString());
    ui->base_plainTextEdit_staff->setPlainText(doc.value("staff").toString());
    ui->base_plainTextEdit_copyright->setPlainText(doc.value("copyright").toString());
    ui->base_comboBox_entrance->setCurrentText(doc.value("entrance").toString());
    ui->base_radioButton_fullScreen->setChecked(doc.value("mode").toString()=="fullscreen");
    ui->base_radioButton_window->setChecked(doc.value("mode").toString()=="window");

}

//标题画面选项卡
void GameConfig::initTitleTab()
{
    //ui->title_preview->setSceneRect(ui->title_preview->rect());
    ui->title_preview->setProjectPath(this->projectPath);

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

    auto bindCheckboxWithButtons = [=](QCheckBox* checkbox, QList<QWidget*> widgets, bool isChecked){
        connect(checkbox,&QCheckBox::stateChanged,[=](int state){
            foreach(QWidget* widget, widgets)
            {
                widget->setDisabled(!state);
            }
        });
        checkbox->setChecked(isChecked);
        if(!isChecked)
        {
            foreach(QWidget* widget, widgets)
            {
                widget->setDisabled(1);
            }
        }
    };



    bindFileOpenWithLineEdit(ui->title_button_bgm,ui->title_lineEdit_bgm);
    bindFileOpenWithLineEdit(ui->title_button_touchSe,ui->title_lineEdit_touchSe);
    bindFileOpenWithLineEdit(ui->title_button_hoverSe,ui->title_lineEdit_hoverSe);
    bindFileOpenWithLineEdit(ui->title_button_bg,ui->title_lineEdit_bg);
    bindFileOpenWithLineEdit(ui->title_button_start,ui->title_lineEdit_start);
    bindFileOpenWithLineEdit(ui->title_button_load,ui->title_lineEdit_load);
    bindFileOpenWithLineEdit(ui->title_button_config,ui->title_lineEdit_config);
    bindFileOpenWithLineEdit(ui->title_button_exit,ui->title_lineEdit_exit);
    bindFileOpenWithLineEdit(ui->title_button_cg,ui->title_lineEdit_cg);
    bindFileOpenWithLineEdit(ui->title_button_music,ui->title_lineEdit_music);
    bindFileOpenWithLineEdit(ui->title_button_about,ui->title_lineEdit_about);

    bindCheckboxWithButtons(ui->title_checkBox_bg,{ui->title_button_bg,ui->title_button_condition_bg},false);
    bindCheckboxWithButtons(ui->title_checkBox_start,{ui->title_button_start,ui->title_button_condition_start},true);
    bindCheckboxWithButtons(ui->title_checkBox_load,{ui->title_button_load,ui->title_button_condition_load},true);
    bindCheckboxWithButtons(ui->title_checkBox_config,{ui->title_button_config,ui->title_button_condition_config},true);
    bindCheckboxWithButtons(ui->title_checkBox_exit,{ui->title_button_exit,ui->title_button_condition_exit},true);
    bindCheckboxWithButtons(ui->title_checkBox_cg,{ui->title_button_cg,ui->title_button_condition_cg},false);
    bindCheckboxWithButtons(ui->title_checkBox_music,{ui->title_button_music,ui->title_button_condition_music},false);
    bindCheckboxWithButtons(ui->title_checkBox_about,{ui->title_button_about,ui->title_button_condition_about},false);


    connect(ui->title_checkBox_bg,&QCheckBox::toggled,[=](bool checked){
        if(checked)
            emit ui->title_lineEdit_bg->textChanged(ui->title_lineEdit_bg->text());
    });
    connect(ui->title_checkBox_start,&QCheckBox::toggled,[=](bool checked){
        if(checked)
            ui->title_preview->setItemImage("ui/"+ui->title_lineEdit_start->text(),50,50);
        else
            ui->title_preview->removeItemImage("ui/"+ui->title_lineEdit_start->text());
    });
    connect(ui->title_checkBox_load,&QCheckBox::toggled,[=](bool checked){
        if(checked)
            ui->title_preview->setItemImage("ui/"+ui->title_lineEdit_load->text(),50,50);
        else
            ui->title_preview->removeItemImage("ui/"+ui->title_lineEdit_load->text());
    });
    connect(ui->title_checkBox_config,&QCheckBox::toggled,[=](bool checked){
        if(checked)
            ui->title_preview->setItemImage("ui/"+ui->title_lineEdit_config->text(),50,50);
        else
            ui->title_preview->removeItemImage("ui/"+ui->title_lineEdit_config->text());
    });
    connect(ui->title_checkBox_exit,&QCheckBox::toggled,[=](bool checked){
        if(checked)
            ui->title_preview->setItemImage("ui/"+ui->title_lineEdit_exit->text(),50,50);
        else
            ui->title_preview->removeItemImage("ui/"+ui->title_lineEdit_exit->text());
    });
    connect(ui->title_checkBox_cg,&QCheckBox::toggled,[=](bool checked){
        if(checked)
            ui->title_preview->setItemImage("ui/"+ui->title_lineEdit_cg->text(),50,50);
        else
            ui->title_preview->removeItemImage("ui/"+ui->title_lineEdit_cg->text());
    });
    connect(ui->title_checkBox_music,&QCheckBox::toggled,[=](bool checked){
        if(checked)
            ui->title_preview->setItemImage("ui/"+ui->title_lineEdit_music->text(),50,50);
        else
            ui->title_preview->removeItemImage("ui/"+ui->title_lineEdit_music->text());
    });
    connect(ui->title_checkBox_about,&QCheckBox::toggled,[=](bool checked){
        if(checked)
            ui->title_preview->setItemImage("ui/"+ui->title_lineEdit_about->text(),50,50);
        else
            ui->title_preview->removeItemImage("ui/"+ui->title_lineEdit_about->text());
    });


    connect(ui->title_lineEdit_bg,&QLineEdit::textChanged,[=](const QString &text){
        if(text.isEmpty())
            ui->title_preview->removeBackgroundImage();
        else
            ui->title_preview->setBackgroundImage("ui/"+ui->title_lineEdit_bg->text());
    });
    connect(ui->title_lineEdit_start,&QLineEdit::textChanged,[=](const QString &text){
        if(text.isEmpty())
            ui->title_preview->removeItemImage(text);
        else
            ui->title_preview->setBackgroundImage("ui/"+ui->title_lineEdit_start->text());
    });

}

void GameConfig::saveDataOfTitleTab()
{

}

void GameConfig::loadDataOfTitleTab()
{

}

//公共函数
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
