#ifndef SOZAIWIDGETPRIVATE_H
#define SOZAIWIDGETPRIVATE_H

#include <QWidget>
#include "ParserHelper.h"

namespace Ui {
class SozaiWidgetPrivate;
}

class SozaiWidgetPrivate : public QWidget
{
    Q_OBJECT

public:
    explicit SozaiWidgetPrivate(QWidget *parent = 0);
    ~SozaiWidgetPrivate();

    void open(const QString &path);

private:
    Ui::SozaiWidgetPrivate *ui;

    QBkeVariable doc;
};

#endif // SOZAIWIDGETPRIVATE_H
