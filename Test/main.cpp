#include "mainwindow.h"
#include <QApplication>
#include "../shared/vshack.h"
#include "NewProjectWizard.h"
#include "SozaiImportWizard.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

//    NewProjectWizard::open();
    SozaiImportWizard::open("D:/workspace/Git/BKOven测试/test");
//    SozaiImportWizard::open("./test");

    return a.exec();
}
