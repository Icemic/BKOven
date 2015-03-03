#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "SozaiWidget/SozaiWidget.h"
#include "StageWidget/StageWidget.h"
#include "ScriptWidget/ScriptWidget.h"
#include "ScriptGeneration/ScriptGenerator.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QToolButton>
#include <QSplitter>
#include <QMessageBox>
#include <QFileDialog>
#include "../shared/vshack.h"
#include "../NewProjectWizard/NewProjectWizard.h"
#include "../SozaiImportWizard/SozaiImportWizard.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    ui->mainToolBar->setDisabled(true);

    QToolButton* newButton = new QToolButton();
    newButton->setText("新建");
    newButton->setIcon(QIcon(":/Sound/stop"));
    newButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    QToolButton* openButton = new QToolButton();
    openButton->setText("打开");
    openButton->setIcon(QIcon(":/Sound/stop"));
    openButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    QToolButton* saveButton = new QToolButton();
    saveButton->setText("保存");
    saveButton->setIcon(QIcon(":/Sound/stop"));
    saveButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    QToolButton* sozaiImportButton = new QToolButton();
    sozaiImportButton->setText("素材库");
    sozaiImportButton->setIcon(QIcon(":/Sound/stop"));
    sozaiImportButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->mainToolBar->addWidget(newButton);
    ui->mainToolBar->addWidget(openButton);
    ui->mainToolBar->addWidget(saveButton);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addWidget(sozaiImportButton);

    QString toolBarStyle = "QToolButton{"
                           "	border: 0;"
                           "	margin: 0;"
                           "	padding: 0;"
                           "	height: 50px;"
                           "	min-width: 52px;"
                           "	color: #000;"
                           "	background-color: #ecedef;"
                           "}"
                           "QToolButton:hover{"
                           "	background-color: #c9dfbb;"
                           "}"
                           "QToolButton:pressed{"
                           "	background-color: #9fc888;"
                           "}";
    ui->mainToolBar->setStyleSheet(toolBarStyle);

    connect(newButton,&QToolButton::clicked,[=](){
        //判断当前项目保存/放弃
        if(!projectPath.isEmpty()){
            int result = QMessageBox::question(this,tr("新建项目"),tr("当前项目尚未保存，是否保存当前进度？"),QMessageBox::Yes,QMessageBox::No,QMessageBox::Cancel);
            if(result==QMessageBox::Yes)
                this->saveProject();
            else if(result==QMessageBox::Cancel)
                return;
        }
        //打开新项目向导
        bool ok = NewProjectWizard::open();
        //判断是否生成了
        if(ok)
            this->openProject(NewProjectWizard::projectPath());

    });
    connect(openButton,&QToolButton::clicked,[=](){
        //判断当前项目保存/放弃
        if(!projectPath.isEmpty()){
            int result = QMessageBox::question(this,tr("新建项目"),tr("当前项目尚未保存，是否保存当前进度？"),QMessageBox::Yes,QMessageBox::No,QMessageBox::Cancel);
            if(result==QMessageBox::Yes)
                this->saveProject();
            else if(result==QMessageBox::Cancel)
                return;
        }

        QString filePath = QFileDialog::getOpenFileName(this,tr("选择打开工程"),".",tr("工程文件 (myGame.bk)"));
        if(filePath.isEmpty())
            return;
        else
            this->openProject(filePath.replace("/myGame.bk",""));

    });
    connect(saveButton,&QToolButton::clicked,[=](){
        this->saveProject();
    });
    connect(sozaiImportButton,&QToolButton::clicked,[=](){
        int ok = SozaiImportWizard::open(projectPath);
        if(ok)
            SozaiWidget::open(projectPath);
    });


    SozaiWidget::init();
    StageWidget::init();
    ScriptWidget::init();

    QWidget* widget = new QWidget();
    QGridLayout *mainlayer = new QGridLayout;
    QSplitter *hsplitter = new QSplitter(Qt::Horizontal);
    QSplitter *vsplitter = new QSplitter(Qt::Vertical);
    vsplitter->addWidget(widget);
    vsplitter->addWidget(StageWidget::widget());
    hsplitter->addWidget(SozaiWidget::widget());
    hsplitter->addWidget(vsplitter);
    hsplitter->addWidget(ScriptWidget::widget());
    hsplitter->setStretchFactor(1,1);
    mainlayer->addWidget(hsplitter);
    ui->centralWidget->setLayout(mainlayer);

    ScriptGenerator::init();

#ifdef QT_DEBUG
    this->openProject("D:/workspace/Git/BKOven测试/test");
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openProject(const QString &projectPath)
{
    this->projectPath = projectPath;
    QFile f(projectPath+"/myGame.bk");
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    projectDoc.loadFromBinary(f.readAll());
    f.close();

    SozaiWidget::open(projectPath);
    ScriptWidget::open(projectPath);

//    ui->mainToolBar->setEnabled(true);
}

void MainWindow::saveProject()
{
    if(projectPath.isEmpty()){
        qWarning() << tr("MainWindow: 项目未打开，故无法保存。") << endl;
        return;
    }

    QFile f(projectPath+"/myGame.bk");
    f.open(QIODevice::WriteOnly | QIODevice::Truncate |QIODevice::Text);
    f.write(projectDoc.saveToBinary());
    f.close();

    ScriptWidget::save();
}

void MainWindow::initConnections()
{

}
