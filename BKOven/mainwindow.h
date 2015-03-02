#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ParserHelper.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void openProject(const QString &projectPath);
    void saveProject();

private:
    Ui::MainWindow *ui;
    QBkeVariable projectDoc;

    QString projectPath;

    void initConnections();

};

#endif // MAINWINDOW_H
