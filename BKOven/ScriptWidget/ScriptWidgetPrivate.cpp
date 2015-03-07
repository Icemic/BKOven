#include "ScriptWidgetPrivate.h"
#include "ScriptTreeWidget.h"
#include "ScriptListWidget.h"
#include "ui_ScriptWidgetPrivate.h"
#include "../ScriptGeneration/ScriptStatus.h"
#include "../StageWidget/StageWidget.h"

ScriptWidgetPrivate::ScriptWidgetPrivate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScriptWidgetPrivate)
{
    ui->setupUi(this);
    ui->scriptTreeWidget->setDisabled(true);
    ui->scriptListWidget->setDisabled(true);

    ScriptStatus::init();

    //加载剧本
    connect(ui->scriptTreeWidget,&ScriptTreeWidget::requestShowScripts,[=](const QString &uuid){
        //缓存当前
        if(!ui->scriptListWidget->uuid().isEmpty())
            sceneScripts[ui->scriptListWidget->uuid()] = ui->scriptListWidget->scriptData();
        ScriptStatus::self()->save();

        //检查要读取的内容是否在缓存中已有
        if(sceneScripts.contains(uuid))
        {
            ui->scriptListWidget->setScriptData(sceneScripts.value(uuid).toBkeArray());
        }
        else
        {
            //读取新剧本
            QBkeVariable doc;
            QFile f(projectPath+"/config/Scene/"+uuid+".bkpsr");
            f.open(QIODevice::ReadOnly | QIODevice::Text);
            doc.loadFromBinary(f.readAll());
            f.close();
            ui->scriptListWidget->setScriptData(doc.toBkeArray());
        }
        ui->scriptListWidget->setUuid(uuid);
        ui->scriptListWidget->setEnabled(true);
        ScriptStatus::self()->open(projectPath,uuid);
        ui->tabWidget->setCurrentIndex(1);
    });

    setupConnections();
}

ScriptWidgetPrivate::~ScriptWidgetPrivate()
{
    delete ui;
}

void ScriptWidgetPrivate::open(const QString &path)
{
    //存储工程路径
    projectPath = path;

    //读取幕与场景的配置文件
    QBkeVariable doc;
    QFile f(path+"/config/ActAndScene.bkpsr");
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    doc.loadFromBinary(f.readAll());
    f.close();
    //清空原有并加载
    ui->scriptTreeWidget->setScriptData(doc.toBkeDic());
    //切换到幕与场景tab
    ui->tabWidget->setCurrentIndex(0);
    ui->scriptTreeWidget->setEnabled(true);
}

void ScriptWidgetPrivate::save()
{
    if(projectPath.isEmpty()){
        qWarning() << tr("ScriptWidgetPrivate: 尚未加载过，因而无法保存。") << endl;
        return;
    }

    //保存幕与场景
    QFile f(projectPath+"/config/ActAndScene.bkpsr");
    f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    f.write(ui->scriptTreeWidget->scriptData().saveToBinary());
    f.close();

    //保存场景脚本
    QDir dir(projectPath);
    dir.mkpath("config/Scene");
    //将最后的缓存写入
    if(!ui->scriptListWidget->uuid().isEmpty())
        sceneScripts[ui->scriptListWidget->uuid()] = ui->scriptListWidget->scriptData();
    ScriptStatus::self()->save();
    //开始保存
    QBkeDictionary::const_iterator ite;
    for(ite=sceneScripts.cbegin();ite!=sceneScripts.cend();ite++)
    {
        QFile f(projectPath+"/config/Scene/"+ite.key()+".bkpsr");
        f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        f.write(ite.value().saveToBinary());
        f.close();
    }
}

bool ScriptWidgetPrivate::addScript(const QString &type, const QBkeVariable &data)
{
    if(!ui->scriptListWidget->isEnabled())
        return false;
    else{
        ui->scriptListWidget->addScript(type,data);
        ui->tabWidget->setCurrentIndex(1);
        return true;
    }
}

void ScriptWidgetPrivate::setupConnections()
{
    connect(ui->scriptListWidget,&ScriptListWidget::scriptClicked,[=](const QString &type, QBkeVariableRef, int row){
        //分析
        ScriptStatus::self()->analysisOne(ui->scriptListWidget,row);
        //更新预览
        //更新舞台区
        StageWidget::setStageStatus(ScriptStatus::self()->currentStatus(row));
    });

    connect(ui->scriptListWidget,&ScriptListWidget::scriptAdded,[=](int row){
        ScriptStatus::self()->dropFrom(row);
        ScriptStatus::self()->analysisOne(ui->scriptListWidget,row);
        //更新预览
        //更新舞台区
        StageWidget::setStageStatus(ScriptStatus::self()->currentStatus(row));
    });

    connect(ui->scriptListWidget,&ScriptListWidget::scriptDeleted,[=](int start,int){
        ScriptStatus::self()->dropFrom(start);
//        ScriptStatus::analysisOne(ui->scriptListWidget,row);
    });

}
