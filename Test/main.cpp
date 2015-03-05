#include "mainwindow.h"
#include <QApplication>
#include "../shared/vshack.h"
#include "NewProjectWizard.h"
#include "SozaiImportWizard.h"

int main(int argc, char *argv[])
{
//    Q_INIT_RESOURCE(icon);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
//    NewProjectWizard::open();
    SozaiImportWizard::open("D:/workspace/Git/BKOven测试/test");
//    SozaiImportWizard::open("./test");
//    ScenarioImportWizard::test();
    return a.exec();
}
