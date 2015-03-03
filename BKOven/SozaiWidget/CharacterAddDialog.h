#ifndef CHARACTERADDDIALOG_H
#define CHARACTERADDDIALOG_H

#include <QDialog>

namespace Ui {
class CharacterAddDialog;
}

class CharacterAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CharacterAddDialog(QWidget *parent = 0);
    ~CharacterAddDialog();

    bool exec(bool &freepos, int &pos, bool &fade, int &from, int &to, int &msec);

private:
    Ui::CharacterAddDialog *ui;
};

#endif // CHARACTERADDDIALOG_H
