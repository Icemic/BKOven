#include "CharacterAddDialog.h"
#include "ui_CharacterAddDialog.h"

CharacterAddDialog::CharacterAddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CharacterAddDialog)
{
    ui->setupUi(this);
}

CharacterAddDialog::~CharacterAddDialog()
{
    delete ui;
}

bool CharacterAddDialog::exec(bool &freepos, int &pos, bool &fade, int &from, int &to, int &msec)
{
    if(QDialog::exec()){
        freepos = ui->radioButton_freePos->isChecked();
        pos = ui->comboBox->currentIndex();
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
