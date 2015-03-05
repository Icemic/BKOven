#ifndef STAGEWIDGETPRIVATE_H
#define STAGEWIDGETPRIVATE_H

#include <QWidget>

namespace Ui {
class StageWidgetPrivate;
}

class StageWidgetPrivate : public QWidget
{
    Q_OBJECT

public:
    explicit StageWidgetPrivate(QWidget *parent = 0);
    ~StageWidgetPrivate();

private:
    Ui::StageWidgetPrivate *ui;
};

#endif // STAGEWIDGETPRIVATE_H
