#include "QLineEditEx.h"

QLineEditEx::QLineEditEx(const QString & contents, QWidget *parent) : QLineEdit(contents, parent)
{

}

void QLineEditEx::setText(const QString &text)
{
    emit textWillChange(this->text(),text);
    QLineEdit::setText(text);
}

void QLineEditEx::setExtraData(const QString &name, const QVariant &data)
{
    _extraData.insert(name,data);
}

const QVariant QLineEditEx::extraData(const QString &name)
{
    return _extraData.value(name);
}
