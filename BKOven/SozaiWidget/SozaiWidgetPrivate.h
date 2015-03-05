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
    void setupConnections();

    Ui::SozaiWidgetPrivate *ui;

    QBkeVariable doc;

private slots:
    void on_add(const QString &type, const QString &name, const QString &filePath);
    void on_quickAdd(const QString &type, const QString &name, const QString &filePath);

signals:
    //void fileAdd(const QString &type, const QString &name, const QString &filePath, bool fade=false, int from=0, int to=255, int msec=800);
    void fileAdd(const QString &type, const QString &name, const QString &filePath, const QBkeVariable &data);
};

#endif // SOZAIWIDGETPRIVATE_H
