#include "GwFirstWindow.h"
#include "ui_GwFirstWindow.h"
#include "GwStudent.h"
#include "GwAssessmentScoreForm.h"
#include "GwMainWindow.h"
#include "GwConnectionDialog.h"
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QDebug>

GwFirstWindow::GwFirstWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GwFirstWindow)
{
    ui->setupUi(this);
}

GwFirstWindow::~GwFirstWindow()
{
    delete ui;
}

void GwFirstWindow::setUpMainWindow()
{   
    auto dialogW = new GwConnectionDialog(this);
    int ret = dialogW->exec();
    if(QDialog::Accepted == ret && !dialogW->connectionData().isEmpty()){
        QMap<QString, QString> classInfoMap = dialogW->connectionData();
        if(!writeFile(dialogW->pathString(), classInfoMap)){
            QMessageBox::critical(this, tr("Saving Error"), tr("Could not save the file"));
            return;
        }
        GwMainWindow *managerWindow = new GwMainWindow(classInfoMap);
        managerWindow->createResultManager();
        managerWindow->show();
        close();
        return;
    }
    return;
}

void GwFirstWindow::openManager()
{
    QString filePath = QDir::homePath() + QString("/ResultManager/");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open a Result Manager"),
                                                    filePath,
                                                    tr("Result Manager Files (*.srm)"));
    if(fileName.isEmpty()) return;
    readFile(fileName);
}

bool GwFirstWindow::writeFile(const QString &path, const QMap<QString, QString> classInfo)
{
    QString filePath = QDir::homePath() + QString("/ResultManager/") + path;
    QDir dirPath(filePath);
    dirPath.mkpath(filePath);
    dirPath.mkdir(filePath);


    QFile file(filePath + "/" + classInfo["class"] + QString(".srm"));
    if(!file.open(QIODevice::WriteOnly)){
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_7);
    out << classInfo;

    file.flush();
    file.close();

    return true;
}


void GwFirstWindow::readFile(const QString &fileName)
{

    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::critical(this, tr("Saving Error"), tr("Could not open the file"));
        return;
    }

    QMap<QString, QString> classInfo;
    QStringList listOfTabs;

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_7);
    in >> classInfo;
    in >> listOfTabs;

    file.close();

    GwMainWindow *mainWin = new GwMainWindow(classInfo, listOfTabs);
    mainWin->openResultManager();
    mainWin->show();
    close();
    return;
}

void GwFirstWindow::on_pushButton_clicked()
{
   setUpMainWindow();
}

void GwFirstWindow::on_actionNewConnection_triggered()
{
    setUpMainWindow();
}

void GwFirstWindow::on_actionOpenConnection_triggered()
{
    openManager();
}

void GwFirstWindow::on_openManagerPushButton_clicked()
{
    openManager();
}
