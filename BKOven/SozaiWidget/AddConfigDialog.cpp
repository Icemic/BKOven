#include "AddConfigDialog.h"
#include "ui_AddConfigDialog.h"

AddConfigDialog::AddConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddConfigDialog)
{
    ui->setupUi(this);


}

AddConfigDialog::~AddConfigDialog()
{
    delete ui;
}

bool AddConfigDialog::exec(bool &fade, int &from, int &to, int &msec)
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
