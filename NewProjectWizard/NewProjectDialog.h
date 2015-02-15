#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>
#include "ParserHelper.h"

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewProjectDialog(QWidget *parent = 0);
    ~NewProjectDialog();

    QBkeVariable projectInfo();

private:
    Ui::NewProjectDialog *ui;

    QBkeVariable doc;


private slots:
    void onAccepted();


};

#endif // NEWPROJECTDIALOG_H
