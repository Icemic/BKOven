#include "ItemAddDialog.h"
#include "ui_ItemAddDialog.h"

ItemAddDialog::ItemAddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ItemAddDialog)
{
    ui->setupUi(this);
}

ItemAddDialog::~ItemAddDialog()
{
    delete ui;
}

bool ItemAddDialog::exec(bool &fade, int &from, int &to, int &msec)
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
