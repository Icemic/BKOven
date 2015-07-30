#ifndef QLINEEDITEX_H
#define QLINEEDITEX_H

#include <QLineEdit>

class QLineEditEx : public QLineEdit
{
    Q_OBJECT
public:
    explicit QLineEditEx(const QString & contents, QWidget *parent = 0);
    explicit QLineEditEx(QWidget *parent = 0):QLineEditEx("",parent){}

    void setExtraData(const QString &name, const QVariant &data);
    const QVariant extraData(const QString &name);

private:
    QVariantMap _extraData;


signals:
    void textWillChange(const QString &prev, const QString &next);

public slots:
    void setText(const QString &text);
};

#endif // QLINEEDITEX_H
