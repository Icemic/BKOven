#include "mainwindow.h"
#include <QApplication>
#include "../shared/vshack.h"
#include "NewProjectWizard.h"
#include "SozaiImportWizard.h"
#include "PreviewWidget.h"
#include "../BKFontText/BKFontText.h"
#include "../shared/vshack.h"
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QImage>
#include <QPixmap>
#include <QHBoxLayout>
#include <QFile>

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

    wi->setCharacterImage("character/ycsn74.png",4);

    wi->setTextWindowImage("ui/textwindow_d/textwindow.png",126,460);

    wi->setTextWindowTextFontInfo("font/SourceHanSansCN-Regular.otf",QColor("#333"),bkeStyleNormal,24);
    wi->setTextWindowTextPos(20,65);
    wi->renderTextWindowText("今晚，可以哊～",985,140);

    wi->setNameBoxTextFontInfo("font/SourceHanSansCN-Regular.otf",QColor("white"),bkeStyleNormal,24);
    wi->setNameBoxTextPos(20,10);
    wi->renderNameBoxText("Taigacon",160);


    wi->connect(wi,&PreviewWidget::itemMoved,[=](const QString &type, const QString &filename, int x, int y){
        qDebug() << type << filename << x << y ;
    });




    return a.exec();
}
