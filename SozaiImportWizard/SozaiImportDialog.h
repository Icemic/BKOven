#ifndef SOZAIIMPORTDIALOG_H
#define SOZAIIMPORTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QListWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
//#include <QOpenGLWidget>
#include "SozaiTreeWidget.h"
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



    void init_image(QGraphicsView *graphicsView, QListWidget *originList, SozaiTreeWidget *targetList, QPushButton *addClassButton, QPushButton *removeClassButton, QPushButton *copyClassButton, QPushButton *clearButton, const QString &topItemName, const QString &folderName);
    void init_sound(QTableWidget *metaDataWidget, QPushButton *metaDataEditButton, QPushButton *metaDataResetButton, QSlider *playProcessSlider, QLabel *playPostionLabel, QPushButton *playButton, QPushButton *pauseButton, QPushButton *stopButton, QListWidget *originList, SozaiTreeWidget *targetList, QPushButton *addClassButton, QPushButton *removeClassButton, QPushButton *copyClassButton, QPushButton *clearButton, const QString &topItemName, const QString &folderName);
    void loadOriginListAndTargetList_image(QDir dir, const QString &folderName, QListWidget *originList, SozaiTreeWidget *targetList);
    void loadOriginListAndTargetList_sound(QDir dir, const QString &folderName, QListWidget *originList, SozaiTreeWidget *targetList);

    QStringList getFileNamesInChildren(QTreeWidgetItem* parent);

private slots:
    void _exportSozaiConfig();  //内部用，按下确定后触发，刷新doc值
    QBkeVariable _exportSozaiConfig_recursive(QTreeWidgetItem* parent);  //递归函数

};

#endif // SOZAIIMPORTDIALOG_H
