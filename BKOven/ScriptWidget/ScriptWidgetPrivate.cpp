#include "ScriptWidgetPrivate.h"
#include "ScriptTreeWidget.h"
#include "ScriptListWidget.h"
#include "ui_ScriptWidgetPrivate.h"

ScriptWidgetPrivate::ScriptWidgetPrivate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScriptWidgetPrivate)
{
    ui->setupUi(this);
    ui->scriptTreeWidget->setDisabled(true);
    ui->scriptListWidget->setDisabled(true);

    //加载剧本
    connect(ui->scriptTreeWidget,&ScriptTreeWidget::requestShowScripts,[=](const QString &uuid){
        //缓存当前
        sceneScripts[ui->scriptListWidget->uuid()] = ui->scriptListWidget->scriptData();

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
    });

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
    //加载
    ui->scriptTreeWidget->setScriptData(doc.toBkeDic());
    //切换到幕与场景tab
    ui->tabWidget->setCurrentIndex(0);
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
    sceneScripts[ui->scriptListWidget->uuid()] = ui->scriptListWidget->scriptData();
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
