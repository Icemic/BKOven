#ifndef ITEMADDDIALOG_H
#define ITEMADDDIALOG_H

#include <QDialog>

namespace Ui {
class ItemAddDialog;
}

class ItemAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ItemAddDialog(QWidget *parent = 0);
    ~ItemAddDialog();

    bool exec(bool &fade, int &from, int &to, int &msec);

private:
    Ui::ItemAddDialog *ui;
};

#endif // ITEMADDDIALOG_H
