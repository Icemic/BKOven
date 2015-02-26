#ifndef SOUNDSELECTDIALOG_H
#define SOUNDSELECTDIALOG_H

#include <QListWidget>
#include "../../shared/vshack.h"
#include "ParserHelper.h"

class SoundSelectDialog : public QListWidget
{
    Q_OBJECT

public:
    explicit SoundSelectDialog(QWidget *parent = 0);
    ~SoundSelectDialog();
    void load(const QString &path, const QString &folder, const QBkeDictionary &dic);

private:
    QMenu *rMenu;
    int playingRow = -1;

    QString projectPath;
    QString resourceFolder;
    QBkeDictionary dic;
    QStringList nodeStack;

    QList<QAction*> fileActions;
    QList<QAction*> folderActions;

    void _load();

protected:
    virtual void closeEvent(QCloseEvent *) override;
    virtual void contextMenuEvent(QContextMenuEvent *) override;

signals:
    void soundSelected(QString absoluteFilePath);
};

#endif // SOUNDSELECTDIALOG_H
