#include "NewProjectDialog.h"
#include "ui_NewProjectDialog.h"

#include <QDebug>


NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(onAccepted()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(reject()));

}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}


void NewProjectDialog::onAccepted()
{
    doc["gameName"] = ui->gameName->text();

    QStringList arr = ui->screenSize->currentText().split(" ");
    QBkeArray arr2;
    arr2 << arr[0].toInt() << arr[2].toInt();

    doc["screenSize"] = arr2;

    doc["projectPath"] = ui->projectPath->text().replace("\\","/") + "/";

    qDebug() << "新建工程对话框：" << doc.saveToString() << endl;

    this->accept();
}

QBkeVariable NewProjectDialog::projectInfo()
{
    return doc;
}
