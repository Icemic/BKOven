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

    QWidget w;
    w.show();

    QHBoxLayout *layer = new QHBoxLayout();

    QLabel *l = new QLabel();
    layer->addWidget(l);
    QPixmap::fromImage(BKFontText::getInstance()->render("test 测试")).save("d:/bke.bmp");
    l->setPixmap(QPixmap::fromImage(BKFontText::getInstance()->render("test 测试")));
    l->setFixedSize(400,300);
    w.setLayout(layer);

//    NewProjectWizard::open();
//    SozaiImportWizard::open("D:/workspace/Git/BKOven测试/test");
//    SozaiImportWizard::open("./test");
//    ScenarioImportWizard::test();


//    PreviewWidget *wi = new PreviewWidget(1280, 720, "D:/workspace/Git/BKOven测试/test");
//    wi->show();
//    wi->setBackgroundImage("background/bg01_2.bmp");

//    wi->setCharacterImage("character/ycsn74.png",200,100);


//    wi->connect(wi,&PreviewWidget::itemMoved,[=](const QString &type, const QString &filename, int x, int y){
//        qDebug() << type << filename << x << y ;
//    });




    return a.exec();
}
