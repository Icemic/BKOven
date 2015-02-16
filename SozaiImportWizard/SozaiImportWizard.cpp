#include "SozaiImportWizard.h"
#include "SozaiImportDialog.h"
#include <QDir>
#include <QFile>
#include <QDebug>

SozaiImportWizard::SozaiImportWizard()
{
}

void SozaiImportWizard::test()
{
    qDebug() << "SozaiImportWizard: ok!" << endl;
}

bool SozaiImportWizard::open(const QString &projectPath)
{

    SozaiImportDialog* dialog = new SozaiImportDialog;
    if(dialog->execWithPath(projectPath)){
        QString docString = dialog->exportSozaiConfig().saveToString();
        QDir dir(projectPath);
        QFile f(dir.absoluteFilePath("config/sozai.bkpsr"));
        f.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate);
        f.write(docString.toUtf8());
        f.close();
        dialog->deleteLater();
        return true;
    }
    else{
        dialog->deleteLater();
        return false;
    }

}

