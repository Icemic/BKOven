#include "BackgroundAddDialog.h"
#include "ui_BackgroundAddDialog.h"

BackgroundAddDialog::BackgroundAddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BackgroundAddDialog)
{
    ui->setupUi(this);
}

BackgroundAddDialog::~BackgroundAddDialog()
{
    delete ui;
}

bool BackgroundAddDialog::exec(bool &fade, int &from, int &to, int &msec)
{
    if(QDialog::exec()){
        fade = ui->radioButton_fade->isChecked();
        from = ui->comboBox_from->currentText().toInt();
        to = ui->comboBox_to->currentText().toInt();
        msec = ui->spinBox_msec->value();
        return true;
    }
    else{
        return false;
    }
}
