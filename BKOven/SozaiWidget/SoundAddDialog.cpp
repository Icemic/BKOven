#include "SoundAddDialog.h"
#include "ui_SoundAddDialog.h"

SoundAddDialog::SoundAddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SoundAddDialog)
{
    ui->setupUi(this);
}

SoundAddDialog::~SoundAddDialog()
{
    delete ui;
}

bool SoundAddDialog::exec(bool &fade, int &from, int &to, int &msec)
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
