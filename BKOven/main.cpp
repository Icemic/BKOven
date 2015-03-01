#include "mainwindow.h"
#include <QApplication>
#include "SozaiWidget/SozaiWidget.h"
#include "StageWidget/StageWidget.h"
#include "ScriptWidget/ScriptWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

//    SozaiWidget::init();
//    SozaiWidget::open("D:/workspace/Git/BKOven测试/test");
//    SozaiWidget::widget()->show();
//    StageWidget::init();
//    StageWidget::test();
//    StageWidget::widget()->show();
    ScriptWidget::init();
    ScriptWidget::test();
    ScriptWidget::widget()->show();

    return a.exec();
}
