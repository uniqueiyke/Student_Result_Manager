#ifndef GWMAINWINDOW_H
#define GWMAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QMap>
#include <QSqlTableModel>
#include <QTabWidget>
#include <QSplitter>
#include <QListWidget>
#include <QListView>
#include <QCloseEvent>
#include <QPrinter>

#include "GwStudentView.h"
#include "GwGradeBookWidget.h"

namespace Ui {
class GwMainWindow;
}

class GwMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    GwMainWindow(const QString &fileName, QWidget *parent = nullptr);
    GwMainWindow(const QMap<QString, QString> &pathStringMap, QWidget *parent = nullptr);
    GwMainWindow(const QMap<QString, QString> &pathStringMap,
                 const QStringList &listOfTabs,  QWidget *parent = nullptr);
    ~GwMainWindow();

    bool openConnection();
    void addStudent();
    void addGreadBook();
    void createResultManager();
    void openResultManager();
    QPair<bool, QString> createStudentTable();
    QString databasePathString();

    void setDatabasePathMap(const QMap<QString, QString> &pathStringMap);
    QMap<QString, QString> databasePathMap();

    void readFile(const QString &fileName);
    bool writeFile(const QString &path, const QMap<QString, QString> classInfo);
    void setupEditorText();
    QString capitalizeEachWord(const QString &string);

    void setListOfTabs(const QStringList &listOfTabs);
    void addStringToTabList(const QString &str);
    QStringList listOfTabs();
    void addListItems();

    void setListItemSelected(const QString &itemString);
    int indexOfTab(const QString &str);
    GwGradeBookWidget *tabWidgetWidget(int index);
    bool isModified();
    bool itemModified();

    void  addToModifiedTabList(const QString &tabText);
    QStringList getModifiedTabListz() const;
    void removeFromModifiedTabList(const QString &tabText);

    // Save Functions for updating database
    bool save(const QString &tabText);
    void saveAll();

    bool ignore(const QString &tabText);
    void ignoreAll();

    void print(QPrinter *printer);
    void filePrintPreview();
    void printDialog();
    void filePrintPdf();

protected:
    void closeEvent(QCloseEvent *e);

signals:
    void studentAdded(const QString &table);
    void listItemDeleted(const QString &table);
    void stringAdded();
    void tabModified(bool);

public slots:
    void open(const QString &fileName);
    void onVHeaderContextMenuRequested(const QPoint &pos);
    void onListWidgetContextMenuRequested(const QPoint &pos);
    void deleteRecord();
    void deleteTable();
    void removeDeletedItemFromList(const QString &table);

private slots:
    void on_action_New_Manager_triggered();
    void on_actionCreate_Grade_Book_triggered();
    void on_actionAdd_Student_triggered();
    void on_actionOpen_Manager_triggered();
    void on_actionUpdate_Quiz_Score_triggered();
    void refreshTable(const QString &table);
    void refreshGradeBookTable(const QString &str);
    void refreshGradeBookTable(int index);
    void closeTab(int index);
    void updateListWidget();
    void changeTab(int index);
    void gotoTab(QListWidgetItem *item);
    void gotoTab1();
    void saveManager();
    void updateGradeBookTables();

    void on_actionSave_triggered();
    bool okToContinue();
    bool yesToSave(int index);
    void tabWidgetModified();

    void on_actionSave_All_triggered();
    void on_actionClose_triggered();
    void on_actionE_xit_triggered();
    void on_actionPrint_triggered();
    void on_actionPrint_Preview_triggered();
    void on_actionExport_PDF_triggered();
    void onCreateResultSlip();
    void on_actionCreateReportSlip_triggered();
    void on_actionCreateResultSheet_triggered();

    void on_actionAbout_triggered();
    void on_actionAbout_Qt_triggered();

private:
    QMap<QString, QString> pathString;
    Ui::GwMainWindow *ui;
    QSqlDatabase db;
    QTabWidget *tabWidget;
    GwStudentView *view;
    QSqlTableModel *model;
    QSplitter *splitter;
    QListWidget *listWidget;
    QStringList tabList;
    QStringList modifiedTabList;
    QPrinter printer;
    QString htmlPath;

    //Private Funtions
    QString tableName(const QString &subjectName);
    void openTab(const QString &str);
    QString studentTableName();
    bool deletionAlert();
    void setHeaderTitles();
    void setHtmlFilePath(const QString &htmlPath);
    QString htmlFilePath() const;
    QTextDocument *createHtmlTableFromModel();

    void writeSetting();
    void readSetting();
};

#endif // GWMAINWINDOW_H
