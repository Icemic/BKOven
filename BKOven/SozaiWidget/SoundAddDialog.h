#ifndef SOUNDADDDIALOG_H
#define SOUNDADDDIALOG_H

#include <QDialog>

namespace Ui {
class SoundAddDialog;
}

class SoundAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SoundAddDialog(QWidget *parent = 0);
    ~SoundAddDialog();

    bool exec(bool &fade, int &from, int &to, int &msec);

private:
    Ui::SoundAddDialog *ui;
};

#endif // SOUNDADDDIALOG_H
