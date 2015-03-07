#include "mainwindow.h"
#include <QApplication>
#include "../shared/vshack.h"
#include "NewProjectWizard.h"
#include "SozaiImportWizard.h"
#include "PreviewWidget.h"
#include "../shared/vshack.h"
#include <QDebug>

int main(int argc, char *argv[])
{
//    Q_INIT_RESOURCE(icon);
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
//    NewProjectWizard::open();
//    SozaiImportWizard::open("D:/workspace/Git/BKOven测试/test");
//    SozaiImportWizard::open("./test");
//    ScenarioImportWizard::test();
    PreviewWidget *wi = new PreviewWidget(1280, 720, "D:/workspace/Git/BKOven测试/test");
    wi->show();
    wi->setBackgroundImage("background/bg01_2.bmp");

    wi->setCharacterImage("character/ycsn74.png",200,100);



    wi->connect(wi,&PreviewWidget::itemMoved,[=](const QString &type, const QString &filename, int x, int y){
        qDebug() << type << filename << x << y ;
    });


    return a.exec();
}
