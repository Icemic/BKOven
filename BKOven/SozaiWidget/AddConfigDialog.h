#ifndef ADDCONFIGDIALOG_H
#define ADDCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class AddConfigDialog;
}

class AddConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddConfigDialog(QWidget *parent = 0);
    ~AddConfigDialog();
    bool exec(bool &fade,int &from, int &to, int &msec);


private:
    Ui::AddConfigDialog *ui;
};

#endif // ADDCONFIGDIALOG_H
