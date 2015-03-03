#ifndef BACKGROUNDADDDIALOG_H
#define BACKGROUNDADDDIALOG_H

#include <QDialog>

namespace Ui {
class BackgroundAddDialog;
}

class BackgroundAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BackgroundAddDialog(QWidget *parent = 0);
    ~BackgroundAddDialog();

    bool exec(bool &fade, int &from, int &to, int &msec);

private:
    Ui::BackgroundAddDialog *ui;
};

#endif // BACKGROUNDADDDIALOG_H
