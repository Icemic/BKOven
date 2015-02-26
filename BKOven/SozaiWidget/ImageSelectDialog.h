#ifndef IMAGESELECTDIALOG_H
#define IMAGESELECTDIALOG_H

#include <QListWidget>
#include <QPainter>
#include "../../shared/vshack.h"
#include "ParserHelper.h"


class ImageSelectDialog : public QListWidget
{
    Q_OBJECT
public:
    explicit ImageSelectDialog(QWidget *parent = 0);
    ~ImageSelectDialog();

    void load(const QString &path, const QString &folder, const QBkeDictionary &dic, bool showPreview=true);

private:
    QWidget *preview;
    QMenu *rMenu;
    bool showPreview = false;

    QBkeDictionary dic;
    QString projectPath;
    QString resourceFolder;
    QStringList nodeStack;

    QList<QAction*> fileActions;
    QList<QAction*> folderActions;

    void _load();

protected:
    virtual void closeEvent(QCloseEvent *e) override;
    virtual void leaveEvent(QEvent *) override;
    virtual void contextMenuEvent(QContextMenuEvent *) override;


signals:
    //void imageSelected(QString absoluteFilePath);
};

#endif // IMAGESELECTDIALOG_H
