#include "mainwindow.h"
#include <QApplication>

#include "../NewProjectWizard/NewProjectWizard.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    NewProjectWizard::open();

    return a.exec();
}
