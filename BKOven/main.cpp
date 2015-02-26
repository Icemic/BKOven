#include "mainwindow.h"
#include <QApplication>
#include "SozaiWidget/SozaiWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    SozaiWidget::init();
    SozaiWidget::open("D:/workspace/Git/BKOven测试/test");
    SozaiWidget::widget()->show();

    return a.exec();
}
