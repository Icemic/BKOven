#include "DescriptionGenerator.h"
#include "../shared/vshack.h"

DescriptionGenerator::DescriptionGenerator()
{

}

DescriptionGenerator::~DescriptionGenerator()
{

}

QString DescriptionGenerator::description(const QString &type, const QBkeVariable &value)
{
    QString des;
    if(type=="background")
        des = tr("背景：")+value.value("name").toString()+" | " + (value.value("fade").toBool()?
                    (tr("淡入 ") + QString::number(value.value("from").toInt()) +"-" + QString::number(value.value("to").toInt())
                     + " " +QString::number(value.value("msec").toInt()) + "ms"):tr("直接出现"));
    else if(type=="character")
        des = tr("立绘：")+value.value("name").toString()+" | " + (value.value("freepos").toBool()?"自由位置":characterPosition(value.value("pos").toInt()))+
                    " | " + (value.value("fade").toBool()?
                    (tr("淡入 ") + QString::number(value.value("from").toInt()) +"-" + QString::number(value.value("to").toInt())
                     + " " +QString::number(value.value("msec").toInt()) + "ms"):tr("直接出现"));
    else if(type=="item")
        des = tr("物件：")+value.value("name").toString()+" | " + (value.value("fade").toBool()?
                    (tr("淡入 ") + QString::number(value.value("from").toInt()) +"-" + QString::number(value.value("to").toInt())
                     + " " +QString::number(value.value("msec").toInt()) + "ms"):tr("直接出现"));
    else if(type=="bgm")
        des = tr("背景音：")+value.value("name").toString()+" | " + (value.value("fade").toBool()?
                    (tr("淡入 ") + QString::number(value.value("from").toInt()) +"-" + QString::number(value.value("to").toInt())
                     + " " +QString::number(value.value("msec").toInt()) + "ms"):tr("直接播放"));
    else if(type=="se")
        des = tr("音效：")+value.value("name").toString()+" | " + (value.value("fade").toBool()?
                    (tr("淡入 ") + QString::number(value.value("from").toInt()) +"-" + QString::number(value.value("to").toInt())
                     + " " +QString::number(value.value("msec").toInt()) + "ms"):tr("直接播放"));

    else
        qWarning() << "DescriptionGenerator: 无效的类型 "+type << endl;

    return des;
}

QString DescriptionGenerator::characterPosition(int index)
{
    static QStringList charaterPositionList;
    if(charaterPositionList.isEmpty())
        charaterPositionList << tr("左边缘") << tr("左方偏左" )<< tr("左方") << tr("中间偏左") << tr("中间" )<<tr( "中间偏右") << tr("右方") << tr("右方偏右") <<tr( "右边缘");
    return charaterPositionList.at(index);
}
