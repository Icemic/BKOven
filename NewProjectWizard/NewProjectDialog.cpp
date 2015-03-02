#include "NewProjectDialog.h"
#include "ui_NewProjectDialog.h"
#include <QFileDialog>
#include <QDebug>
#include "../shared/vshack.h"

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(onAccepted()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(reject()));
    connect(ui->selectPathButton,&QPushButton::clicked,[=](){
        QString path = QFileDialog::getExistingDirectory(this,tr("选择目录"),".");
        if(!path.isEmpty()){
            ui->projectPath->setText(path);
        }
    });

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
