#ifndef NEWPROJECTWIZARD_H
#define NEWPROJECTWIZARD_H

#include <QtCore>

class NewProjectWizard {

public:
    explicit NewProjectWizard();

    static void test();    //测试用，返回“ok！”

    static bool open();    //启动新建工程对话框，并创建工程
    static QString projectFilePath();//获得工程文件 myGame.bk 的路径


private:
    static QString _projectFilePath;

};

#endif // NEWPROJECTWIZARD_H
