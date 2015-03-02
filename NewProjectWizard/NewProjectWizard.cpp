#include "NewProjectWizard.h"

#include "NewProjectDialog.h"
#include "ParserHelper.h"
#include <QDir>
#include <QDebug>
#include "../shared/vshack.h"

QString NewProjectWizard::_projectFilePath = "";
QString NewProjectWizard::_projectPath = "";
NewProjectWizard::NewProjectWizard()
{

}

//测试
void NewProjectWizard::test()
{

    qDebug() << "NewProjectWizard: ok!" << endl;

}

//启动新建工程对话框，并创建工程
bool NewProjectWizard::open()
{
    NewProjectDialog* dialog = new NewProjectDialog;
    if (dialog->exec())
    {
        dialog->deleteLater();

        QBkeVariable doc = dialog->projectInfo();
        QString projectPath = doc["projectPath"].toString() + doc["gameName"].toString();   //工程文件所在目录，doc["projectPath"]实际上是工作区目录
        QDir dir(projectPath);
        dir.mkpath(projectPath);

        dir.mkdir("Data");
        dir.cd("Data");
        dir.mkdir("background");
        dir.mkdir("character");
        dir.mkdir("item");
//        dir.mkdir("button");
        dir.mkdir("ui");
        dir.mkdir("bgm");
        dir.mkdir("se");
        dir.mkdir("voice");
        dir.cd("../");
        dir.mkdir("config");

        QBkeVariable projectData;
        projectData["gameName"] = doc["gameName"];
        projectData["screenSize"] = doc["screenSize"];

//        projectData["gameName"].redirect( doc["gameName"] );
//        projectData["screenSize"].redirect( doc["screenSize"] );

        QFile f(dir.absoluteFilePath("myGame.bk"));
        f.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
        f.write(projectData.saveToString().toUtf8());
        f.close();

        _projectFilePath = dir.absoluteFilePath("myGame.bk");
        _projectPath = dir.absolutePath();

        return true;
    }
    else
    {
        _projectFilePath.clear();

        dialog->deleteLater();

        return false;
    }
}

//获得工程文件 myGame.bk 的路径
QString NewProjectWizard::projectFilePath()
{
    return _projectFilePath;
}

QString NewProjectWizard::projectPath()
{
    return _projectPath;
}
