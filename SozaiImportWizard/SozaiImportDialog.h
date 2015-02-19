#ifndef SOZAIIMPORTDIALOG_H
#define SOZAIIMPORTDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include "../ParserHelper/ParserHelper.h"

namespace Ui {
class SozaiImportDialog;
}

class SozaiImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SozaiImportDialog(QWidget *parent = 0);
    ~SozaiImportDialog();

    bool execWithPath(const QString &path);
    QBkeVariable exportSozaiConfig(){
        return doc;
    }

private:
    Ui::SozaiImportDialog *ui;

    QString projectDataPath;
    QBkeVariable doc;

    QStringList recursiveSearch(const QString &path, const QString &relativePath, const QStringList &nameFilters);



    void init_bg();

    QStringList getFileNamesInChildren(QTreeWidgetItem* parent);

//    bool isEditting = false;

private slots:
    void _exportSozaiConfig();  //内部用，按下确定后触发，刷新doc值

};

#endif // SOZAIIMPORTDIALOG_H
