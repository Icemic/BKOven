#ifndef SOZAIIMPORTWIZARD_H
#define SOZAIIMPORTWIZARD_H

#include <QtCore>

class SozaiImportWizard {

public:
    explicit SozaiImportWizard();

    static void test();

    static bool open(const QString &projectPath);

};



#endif // SOZAIIMPORTWIZARD_H
