#include "GwMainWindow.h"
#include "ui_GwMainWindow.h"

#include <QInputDialog>
#include <QLabel>
#include <QtSql>
#include <QMessageBox>
#include <QDir>
#include <QPair>
#include <QFileDialog>
#include <QTimer>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QPainter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrintPreviewWidget>
#include <QDesktopWidget>

#include "GwStudent.h"
#include "GwAssessmentScoreForm.h"
#include "GwConnectionDialog.h"
#include "GwStudentView.h"
#include "GwGradeBookWidget.h"
#include "GwQueryDialog.h"
#include "GwResult.h"
#include "GwResultSheet.h"

#ifdef Q_OS_MAC
const QString rsrcPath = ":/mac/images/images/mac";
#else
const QString rsrcPath = ":/win/images/images/win";
#endif

GwMainWindow::GwMainWindow(const QString &fileName, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::GwMainWindow)
{
    ui->setupUi(this);

    readSetting();

    open(fileName);
    openConnection();

    tabWidget = new QTabWidget;
    tabWidget->setTabsClosable(true);

    listWidget = new QListWidget;
    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    splitter = new QSplitter;
    splitter->setOrientation(Qt::Horizontal);

    QList<int> intList;
    intList << ((width()/10)*2) << ((width()/10)*8); // Sizes for the two area of the splitter

    splitter->addWidget(listWidget);
    splitter->addWidget(tabWidget);
    splitter->setCollapsible(0, true);
    splitter->setSizes(intList);

    setCentralWidget(splitter);

    view = new GwStudentView;
    model = new QSqlTableModel(this, db);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    view->textEditor()->setReadOnly(true);
    view->textEditor()->setStyleSheet(QString("border: none"));
    view->tableViewer()->setStyleSheet(QString("border: none"));
    view->tableViewer()->setAlternatingRowColors(true);
    view->tableViewer()->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    view->tableViewer()->setModel(model);

    updateListWidget();

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GwMainWindow::tabWidgetModified);
    timer->start(5000);

    connect(this, &GwMainWindow::studentAdded, this, &GwMainWindow::refreshTable);
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &GwMainWindow::closeTab);
    connect(this, &GwMainWindow::stringAdded, this, &GwMainWindow::updateListWidget);
    connect(this, &GwMainWindow::stringAdded, this, &GwMainWindow::saveManager);
    connect(tabWidget, &QTabWidget::currentChanged, this, &GwMainWindow::changeTab);
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &GwMainWindow::gotoTab);
    connect(view->tableViewer()->verticalHeader(), &QHeaderView::customContextMenuRequested, this, &GwMainWindow::onVHeaderContextMenuRequested);
    connect(listWidget, &QListWidget::customContextMenuRequested, this, &GwMainWindow::onListWidgetContextMenuRequested);
}

GwMainWindow::GwMainWindow(const QMap<QString, QString> &pathStringMap, QWidget *parent) :
    QMainWindow(parent), pathString(pathStringMap), ui(new Ui::GwMainWindow)
{
    ui->setupUi(this);
    readSetting();

    openConnection();

    tabWidget = new QTabWidget;
    tabWidget->setTabsClosable(true);

    listWidget = new QListWidget;
    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    listWidget->setEditTriggers(QAbstractItemView::EditKeyPressed);

    splitter = new QSplitter;
    splitter->setOrientation(Qt::Horizontal);

    QList<int> intList;
    intList << ((width()/10)*2) << ((width()/10)*8); // Sizes for the two area of the splitter

    splitter->addWidget(listWidget);
    splitter->addWidget(tabWidget);
    splitter->setCollapsible(0, true);
    splitter->setSizes(intList);

    setCentralWidget(splitter);

    view = new GwStudentView;
    model = new QSqlTableModel(this, db);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    view->textEditor()->setReadOnly(true);
    view->textEditor()->setStyleSheet(QString("border: none"));

    view->tableViewer()->setStyleSheet(QString("border: none"));
    view->tableViewer()->setAlternatingRowColors(true);
    view->tableViewer()->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    view->tableViewer()->setModel(model);

    updateListWidget();

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GwMainWindow::tabWidgetModified);
    timer->start(5000);

    connect(this, &GwMainWindow::studentAdded, this, &GwMainWindow::refreshTable);
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &GwMainWindow::closeTab);
    connect(this, &GwMainWindow::stringAdded, this, &GwMainWindow::updateListWidget);
    connect(this, &GwMainWindow::stringAdded, this, &GwMainWindow::saveManager);
    connect(tabWidget, &QTabWidget::currentChanged, this, &GwMainWindow::changeTab);
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &GwMainWindow::gotoTab);
    connect(view->tableViewer()->verticalHeader(), &QHeaderView::customContextMenuRequested, this, &GwMainWindow::onVHeaderContextMenuRequested);
    connect(listWidget, &QListWidget::customContextMenuRequested, this, &GwMainWindow::onListWidgetContextMenuRequested);
}

GwMainWindow::GwMainWindow(const QMap<QString, QString> &pathStringMap, const QStringList &listOfTabs, QWidget *parent)
    :QMainWindow(parent), pathString(pathStringMap), ui(new Ui::GwMainWindow), tabList(listOfTabs)
{
    ui->setupUi(this);
    readSetting();

    openConnection();
    tabWidget = new QTabWidget;
    tabWidget->setTabsClosable(true);


    listWidget = new QListWidget;
    listWidget->setSortingEnabled(true);
    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    addListItems(); //! may be removed

    splitter = new QSplitter;
    splitter->setOrientation(Qt::Horizontal);

    QList<int> intList;
    intList << ((width()/10)*2) << ((width()/10)*8);

    splitter->addWidget(listWidget);
    splitter->addWidget(tabWidget);
    splitter->setCollapsible(0, true);
    splitter->setSizes(intList);

    setCentralWidget(splitter);

    view = new GwStudentView;
    model = new QSqlTableModel(this, db);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    view->textEditor()->setReadOnly(true);
    view->textEditor()->setStyleSheet(QString("border: none"));

    view->tableViewer()->setStyleSheet(QString("border: none"));
    view->tableViewer()->setAlternatingRowColors(true);
    view->tableViewer()->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    view->tableViewer()->setModel(model);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GwMainWindow::tabWidgetModified);
    timer->start(5000);

    connect(this, &GwMainWindow::studentAdded, this, &GwMainWindow::refreshTable);
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &GwMainWindow::closeTab);
    connect(this, &GwMainWindow::stringAdded, this, &GwMainWindow::updateListWidget);
    connect(this, &GwMainWindow::stringAdded, this, &GwMainWindow::saveManager);
    connect(tabWidget, &QTabWidget::currentChanged, this, &GwMainWindow::changeTab);
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &GwMainWindow::gotoTab);
    connect(view->tableViewer()->verticalHeader(), &QHeaderView::customContextMenuRequested, this, &GwMainWindow::onVHeaderContextMenuRequested);
    connect(listWidget, &QListWidget::customContextMenuRequested, this, &GwMainWindow::onListWidgetContextMenuRequested);
}

GwMainWindow::~GwMainWindow()
{
    delete ui;
}

//------------------------------------------------------------------------------------------
bool GwMainWindow::openConnection()
{
    QString dbPath = databasePathString() + QString("/database");

    if(dbPath.isEmpty()){
        QMessageBox::critical(this, tr("Path Error"),
                              tr("The manager could not be created"
                                 "The database path is empty\n"
                                 "Please fill in the informations properly"));
        return false;
    }
    QDir database(dbPath);     //!Create a pointer to the directory

    //!Make a diretory for SQLite Database
    database.mkpath(dbPath);
    database.mkdir(dbPath);

    //!Make a database conection
    const QMap<QString, QString> pathStringMap = databasePathMap();
    QString connectStr = database.path() + QString("/%1%2").arg(pathStringMap["class"]).arg(QString(".db"));
    QString connecName = pathStringMap["class"] + pathStringMap["term"];
    db = QSqlDatabase::addDatabase("QSQLITE", connecName);
    db.setDatabaseName(connectStr);

    if (!db.open()) {
        QMessageBox::warning(nullptr, QObject::tr("Database Error"),
                             db.lastError().text());
        return false;
    }
    return true;
}

void GwMainWindow::addStudent()
{
    GwStudent addStudentDialog;
    int ret = addStudentDialog.exec();
    if(QDialog::Accepted == ret){
        QString regNum =  addStudentDialog.registrationNumber();
        QString surname = addStudentDialog.lastName();
        QString name = addStudentDialog.firstName();
        QString otherName = addStudentDialog.middleName();
        QString gender = addStudentDialog.studentGender();

        QSqlQuery query(db);
        QString preQuery = ("INSERT INTO students (reg_num, surname, first_name,"
                            "middle_name, gender) "
                "VALUES (:reg_num, :surname, :first_name, :middle_name, :gender)");

        if(db.open() && query.prepare(preQuery)){
            query.bindValue(":reg_num", regNum);
            query.bindValue(":surname", surname);
            query.bindValue(":first_name", name);
            query.bindValue(":middle_name", otherName);
            query.bindValue(":gender", gender);
            if(query.exec()){
                emit studentAdded(QString("students"));
                updateGradeBookTables();
            }else{
                QMessageBox::warning(nullptr, QObject::tr("Failed"),
                                     QString(tr("Could not add the student record\n"))
                                     + query.lastError().text());
            }
        }else{
            QMessageBox::warning(nullptr, QObject::tr("Failed"),
                                 QString(tr("Could not add the student record\n"))
                                 + query.lastError().text());
        }
    }
}

void GwMainWindow::addGreadBook()
{
    QSqlQuery query(db);

    bool yes;
    QString subjName = QInputDialog::getText(this, tr("QInputDialog::getText()"),
               tr("Enter The Subject Name\n"
                  "(e.g. english)"), QLineEdit::Normal,
               QString(""), &yes);

    if(subjName.isEmpty()){
        return;
    }

    QString subjectName = tableName(subjName);
    if(db.isOpen()){
        //!Create a table
        bool y = query.exec(QString("CREATE TABLE  %1("
           "reg_num TEXT UNIQUE NOT NULL,"
           "surname VARCHAR(40),"
           "first_name VARCHAR(40),"
           "first_quiz INT,"
           "second_quiz INT,"
           "third_quiz INT,"
           "other_assessment INT,"
           "examination INT,"
           "total INT, position VARCHAR(10))").arg(subjectName));
        if(!y){
            QMessageBox::warning(nullptr, QObject::tr("Failed"),
                                 QString(tr("Could not create the grade book record\n"))
                                 + query.lastError().text());
            return;
        }
    }

    //!Sort the student table bofore quring it
    query.exec(QString("SELECT * FROM students ORDER BY reg_rum ASC"));

    //!Fetch the reg number,  surname and first name of students in the class
    QString queryStr = ("SELECT reg_num, surname, first_name FROM students");
    if(query.exec(queryStr)){
        while (query.next()) {
            //! Iterate through the names in each record and store the values
            //! in the variables
            QString regNum = query.value("reg_num").toString();
            QString surname = query.value("surname").toString();
            QString name = query.value("first_name").toString();

            //! Construct a query string
            QString preQuery = QString("INSERT INTO %1(reg_num, surname, first_name) "
                    "VALUES (:reg_num, :surname, :first_name)").arg(subjectName);

            //! create a QSqlQuery object and prepare a query
            QSqlQuery secQuery(db);
            secQuery.prepare(preQuery);
            secQuery.bindValue(":reg_num", regNum);
            secQuery.bindValue(":surname", surname);
            secQuery.bindValue(":first_name", name);
            secQuery.exec();
        }
        openTab(subjName);
    }else{
        QMessageBox::warning(nullptr, QObject::tr("Failed"),
                             QString(tr("Could not create the grade book record\n"))
                             + query.lastError().text());
    }
}


//---------------------------------------------------------------------------

void GwMainWindow::createResultManager()
{
      if(db.isOpen() && createStudentTable().first){
          QMessageBox::information(nullptr, QObject::tr("Success"),
                               QString(tr("The Maneger has been created\n"
                               "The Students table has also been created.\n"
                               "You can start adding students to the manager")));

      }else{
          QMessageBox::warning(nullptr, QObject::tr("Failed"),
                               QObject::tr("Could not create the manager and the table.\n"
                                           "Result manager with the name already exists \n And ")
                               + createStudentTable().second);
      }
}

//! This fun is called in the readFile function to open already existing
//! manager file saved in the external drive.
void GwMainWindow::openResultManager()
{
    refreshTable("students");
    int index = tabWidget->addTab(view, studentTableName());
    tabWidget->setCurrentIndex(index);
    setupEditorText();
}

//! This function is called in on_action_New_Manager_triggered() slot
//! for creating a fresh manager for a new class when create manager is click on
//! File New menu of or on the tool bar.
QPair<bool, QString> GwMainWindow::createStudentTable()
{
    QSqlQuery query(db);
    bool ok = query.exec("CREATE TABLE students ("
               "reg_num TEXT UNIQUE NOT NULL, "
               "surname VARCHAR(40) NOT NULL,"
               "first_name VARCHAR(40) NOT NULL,"
               "middle_name VARCHAR(40), "
               "gender VARCHAR(10) NOT NULL)");

    QString studTableName = studentTableName();
    model->setTable("students");
    tabWidget->addTab(view, studTableName);
    addStringToTabList(studTableName);
    setupEditorText();

    QPair<bool, QString> pair = qMakePair(ok, query.lastError().text());
    return pair;
}

void GwMainWindow::setDatabasePathMap(const QMap<QString, QString> &pathStringMap)
{
    pathString = pathStringMap;
}

QString GwMainWindow::databasePathString()
{
    if(databasePathMap().isEmpty()){
        return QString();
    }
    const QMap<QString, QString> pathStringMap = databasePathMap();
    //!Create a directory path for the SQLite databse
    QString dirc = QDir::homePath();
    QString dbPath = dirc + QString("/ResultManager/%1/%2Term/%3").arg(pathStringMap["year"])
            .arg(pathStringMap["term"]).arg(pathStringMap["class"]);

    return dbPath;
}

QMap<QString, QString> GwMainWindow::databasePathMap()
{
    return pathString;
}


void GwMainWindow::on_action_New_Manager_triggered()
{   
    auto dialogW = new GwConnectionDialog;
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
        return;
    }
    return;
}

void GwMainWindow::on_actionCreate_Grade_Book_triggered()
{
    //Create a grade book
    addGreadBook();
}

void GwMainWindow::on_actionAdd_Student_triggered()
{
    //add student call
    addStudent();
}

void GwMainWindow::on_actionOpen_Manager_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open a Result Manager"),
                                                    QString(tr("%1")).arg(databasePathString()),
                                                    tr("Result Manager Files (*.srm)"));
    readFile(fileName);
}

void GwMainWindow::on_actionUpdate_Quiz_Score_triggered()
{
    GwAssessmentScoreForm scoreForm;
    QStringList list = listOfTabs();
    list.removeOne(studentTableName());
    scoreForm.setSubjectList(list);
    int ret = scoreForm.exec();
    if(QDialog::Accepted == ret){
        QString regNum =  "\"" + scoreForm.registrationNumber() + "\"";
        QString subject = tableName(scoreForm.subject());
        QString firstQuiz = scoreForm.firstQuizScore();
        QString SecondQuiz = scoreForm.secondQuizScore();
        QString thirdQuiz = scoreForm.thirdQuizScore();
        QString otherAss = scoreForm.otherAssQuizScore();
        QString exam = scoreForm.examScore();

        QSqlQuery query(db);
        QString preQuery = QString("UPDATE  %1 SET first_quiz = %2, "
                                   "second_quiz = %3, third_quiz = %4, "
                                   "other_assessment = %5, examination = %6 "
                                   " WHERE reg_num = %7").arg(subject)
                .arg(firstQuiz).arg(SecondQuiz).arg(thirdQuiz).arg(otherAss).arg(exam).arg(regNum);

        if(db.open() && query.exec(preQuery)){
            int numberOfOpenedTabs = tabWidget->count();
            for(int i = 0; i < numberOfOpenedTabs; ++i){
                GwGradeBookWidget *gradeBook = tabWidgetWidget(i);
                if(gradeBook){
                    refreshGradeBookTable(i);
                }
            }

            QMessageBox::information(nullptr, QObject::tr("Success"),
                                 QString(tr("The Student Record Updated")));
        }else{
            QMessageBox::warning(nullptr, QObject::tr("Failed"),
                                 QString(tr("Could not Update the student record\n"))
                                 + query.lastError().text());
        }
    }
}

void GwMainWindow::readFile(const QString &fileName)
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
    return;
}

bool GwMainWindow::writeFile(const QString &path, const QMap<QString, QString> classInfo)
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

void GwMainWindow::setupEditorText()
{
    QString className = databasePathMap()["class"].replace("_", " ");
    QString term = databasePathMap()["term"];
    QString year = databasePathMap()["year"].replace("_", "/");

    QString text = QString(tr("<h1 align=\"center\">EMAK GOD'S OWN SCHOOLS</h1>"
                   "<h2 align=\"center\">20 OJIKE STREET, UMUAHIA, ABIA STATE</h2>"
                   "<h3 align=\"center\"> %1 STUDENTS %2.  %3 TERM</h3>")).arg(className)
            .arg(year).arg(term);

    view->textEditor()->setHtml(text);
}

QString GwMainWindow::capitalizeEachWord(const QString &string)
{
    QString str = string;
    QStringList strList = str.split(" ");
    for(int i = 0, c = strList.count(); i < c; ++i) {
        QString s = strList.at(i);
        QString u = s.mid(0,1).toUpper();
        QString l = s.mid(1).toLower();
        QString ul = u + l;
        strList.replace(i, ul);
    }

    return strList.join(" ");
}

void GwMainWindow::setListOfTabs(const QStringList &listOfTabs)
{
    tabList = listOfTabs;
}

void GwMainWindow::addStringToTabList(const QString &str)
{
    if(!tabList.contains(str, Qt::CaseInsensitive)){
        tabList += str;
        emit stringAdded();
    }
}


QStringList GwMainWindow::listOfTabs()
{
    return tabList;
}

//! This function is in dout
void GwMainWindow::addListItems()
{
    const QStringList itemsList = listOfTabs();
    listWidget->addItems(itemsList);
}

void GwMainWindow::setListItemSelected(const QString &itemString)
{
    QList<QListWidgetItem *> itemPtr = listWidget->findItems(itemString, Qt::MatchFixedString);
    if(itemPtr.isEmpty())
        return;
    QListWidgetItem *item = itemPtr.first();
    listWidget->setCurrentItem(item);
}

void GwMainWindow::refreshTable(const QString &table)
{
    model->setTable(table);
    setHeaderTitles();
    model->select();
}

void GwMainWindow::refreshGradeBookTable(const QString &str)
{
    int index = indexOfTab(str);
    GwGradeBookWidget *gradeBook = tabWidgetWidget(index);
    if(gradeBook){
        gradeBook->refreshView(tableName(str));
    }
}

void GwMainWindow::refreshGradeBookTable(int index)
{
    QString str = tabWidget->tabText(index);
    GwGradeBookWidget *gradeBook = tabWidgetWidget(index);
    if(gradeBook){
        gradeBook->refreshView(tableName(str));
    }
}

void GwMainWindow::closeTab(int index)
{
    if(!yesToSave(index))
        return;

    tabWidget->removeTab(index);
}

void GwMainWindow::updateListWidget()
{
    if(!listOfTabs().isEmpty()){
        QString lastStr = listOfTabs().last();
        QStringList list = listOfTabs();
        list.sort(Qt::CaseInsensitive);
        listWidget->clear();
        listWidget->addItems(list);
        setListItemSelected(lastStr);
    }

}

void GwMainWindow::changeTab(int index)
{
    Q_UNUSED(index)
    int i = tabWidget->currentIndex();
    QString tabText = tabWidget->tabText(i);
    setListItemSelected(tabText);
}

void GwMainWindow::gotoTab(QListWidgetItem *item)
{
    QString str = item->text();
    int index = indexOfTab(str);
    if(-1 != index){
        tabWidget->setCurrentIndex(index);
        return;
    }

    QString table = studentTableName();
    if(str == table){
        model->setTable("students");
        model->select();
        int index = tabWidget->addTab(view, table);
        tabWidget->setCurrentIndex(index);
        setupEditorText();
    }else{
        openTab(str);
    }
}

void GwMainWindow::gotoTab1()
{
    QListWidgetItem *item = listWidget->currentItem();
    gotoTab(item);
}

QString GwMainWindow::tableName(const QString &subjectName)
{
    QString str = subjectName;
    return str.replace(" ", "_").toLower();
}

void GwMainWindow::openTab(const QString &str)
{
    auto gradeBook = new GwGradeBookWidget(db, this);
    QString subjectName = capitalizeEachWord(str);

    int nextTabIndex = tabWidget->addTab(gradeBook, QString("%1").arg(subjectName));
    tabWidget->setCurrentIndex(nextTabIndex);

    gradeBook->setupEditorText(subjectName, databasePathMap());
    gradeBook->refreshView(tableName(str));

    addStringToTabList(subjectName);

}

QString GwMainWindow::studentTableName()
{
    return QString(tr("%1 Students")).arg(databasePathMap()["class"]).replace("_", " ");
}

bool GwMainWindow::deletionAlert()
{
    int ret = QMessageBox::question(this, tr("Error"), tr("The student record deleted from this table "
                                                "will also be deleted from all the GradeBooks.\n "
                                                "Deletion of records cannot be undo\n"
                                                "Do you want to continue with the deletion?"),
                          QMessageBox::Yes | QMessageBox::No);
    if(QMessageBox::No == ret)
        return false;
    return true;
}

void GwMainWindow::setHeaderTitles()
{
    model->setHeaderData(0, Qt::Horizontal, tr("Reg. Number"));
    model->setHeaderData(1, Qt::Horizontal, tr("Surname"));
    model->setHeaderData(2, Qt::Horizontal, tr("First Name"));
    model->setHeaderData(3, Qt::Horizontal, tr("Middle Names"));
    model->setHeaderData(4, Qt::Horizontal, tr("Gender"));
}

void GwMainWindow::setHtmlFilePath(const QString &htmlPath1)
{
    htmlPath = htmlPath1;
}

QString GwMainWindow::htmlFilePath() const
{
    return htmlPath;
}



int GwMainWindow::indexOfTab(const QString &str){
    for(int index = 0, j = tabWidget->count(); index < j; ++index){
        if(tabWidget->tabText(index) == str){
            return index;
        }
    }

    return -1;
}

GwGradeBookWidget *GwMainWindow::tabWidgetWidget(int index)
{
    return dynamic_cast<GwGradeBookWidget *>(tabWidget->widget(index));
}

bool GwMainWindow::isModified()
{
    if(!modifiedTabList.isEmpty())
        return true;
    return false;
}

void GwMainWindow::closeEvent(QCloseEvent *e)
{
    if(okToContinue()){
        writeSetting();
        e->accept();
    }else {
        e->ignore();
    }
}

void GwMainWindow::open(const QString &fileName)
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

    setDatabasePathMap(classInfo);
    setListOfTabs(listOfTabs);
}

void GwMainWindow::onVHeaderContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    QAction *deleteAction = new QAction(tr("delete row"), this);
    connect(deleteAction, &QAction::triggered, this, &GwMainWindow::deleteRecord);
    menu.addAction(deleteAction);

    QAction *addRecordAction = new QAction(tr("add student"), this);
    connect(addRecordAction, &QAction::triggered, this, &GwMainWindow::addStudent);
    menu.addAction(addRecordAction);

    QModelIndex index = view->tableViewer()->verticalHeader()->currentIndex();
    QSqlRecord record = model->record(index.row());
    QString surname = record.value("surname").toString();
    QString firstName = record.value("first_name").toString();

    QAction *createResultAction = new QAction(tr("Create Result Slip %1 %2").arg(surname).arg(firstName), this);
    createResultAction->setToolTip(tr("Create Result Slip for %1 %2").arg(surname).arg(firstName));
    connect(createResultAction, &QAction::triggered, this, &GwMainWindow::onCreateResultSlip);
    menu.addAction(createResultAction);

    menu.exec(view->tableViewer()->verticalHeader()->viewport()->mapToGlobal(pos));
}

void GwMainWindow::onListWidgetContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    QAction *openAction = new QAction("open", this);
    connect(openAction, &QAction::triggered, this, &GwMainWindow::gotoTab1);
    menu.addAction(openAction);

    QAction *editAction = new QAction("Delete", this);
    connect(editAction, &QAction::triggered, this, &GwMainWindow::deleteTable);
    menu.addAction(editAction);


    QAction *addRecordAction = new QAction("add student", this);
    if(studentTableName() != listWidget->currentItem()->text()){
        addRecordAction->setVisible(false);
    }
    connect(addRecordAction, &QAction::triggered, this, &GwMainWindow::addStudent);
    menu.addAction(addRecordAction);

    menu.exec(listWidget->viewport()->mapToGlobal(pos));
}

void GwMainWindow::deleteRecord()
{
    QModelIndex index = view->tableViewer()->verticalHeader()->currentIndex();
    QSqlRecord record = model->record(index.row());
    QString regNum = "\"" + record.value("reg_num").toString() + "\"";

    QStringList tables = listOfTabs();
    QString tName;

    if(!deletionAlert())
        return;
    db.transaction();
    foreach (QString table, tables) {
        if(table == studentTableName())
            tName = "students";
        else
            tName = tableName(table);
        QSqlQuery query(db);
        QString queryStr = QString("DELETE FROM %1 WHERE reg_num = %2").arg(tName).arg(regNum);
        if(!query.exec(queryStr))
            QMessageBox::critical(this, tr("Deletion Error"), tr("Could not delete %1 from "
                                                                 "%2\n%3")
                                  .arg(regNum).arg(table).arg(query.lastError().text()));
    }
    if(!db.commit()){
        QMessageBox::critical(this, tr("Deletion Error"), tr("The deletion of student with the %1"
                                                             "from the ResultManager could not be completed. "
                                                             "\n%2")
                              .arg(regNum).arg(db.lastError().text()));
    }else{
        int numberOfOpenedTbs = tabWidget->count();
        for(int i = 0; i < numberOfOpenedTbs; ++i){
            GwGradeBookWidget *gradeBook = tabWidgetWidget(i);
            if(gradeBook){
                refreshGradeBookTable(i);
            }else{
                refreshTable("students");
            }
        }
    }
}

void GwMainWindow::deleteTable()
{
    if(listWidget->currentItem()->text() == studentTableName()){
        QMessageBox::critical(this, tr("Not Allowed "),
                              tr("%1 Table cannot be deleted. Deleting this table"
                                 " means crashing the ResultManager database.\nIf the "
                                 "the Manager is no longer needed, delete the file from the folder."));
        return;
    }

    QListWidgetItem *item = listWidget->currentItem();

    QString itemText = item->text();
    QString table = tableName(itemText);
    int tabIndex = indexOfTab(itemText);

    db.transaction();
    QSqlQuery query(db);
    QString queryStr = QString("DROP TABLE %1").arg(table);
    if(db.tables().contains(table, Qt::CaseInsensitive)){
        int ret = QMessageBox::warning(this, tr("Expensive Operation") , tr("Do You want to delete this GradeBook?\n"
                                      "Deletion of GradeBooks cannot be undo\n"
                                      "Do you want to continue?"),
                             QMessageBox::Yes | QMessageBox::No);
        if(QMessageBox::No == ret)
            return;
        query.exec(queryStr);
        db.commit();

        if(-1 != tabIndex)
            closeTab(tabIndex);
        QMessageBox::information(this, tr("Deleted"), tr("The %1 GradeBook has been "
                                                         "successfully deleted").arg(itemText));
        delete listWidget->takeItem(listWidget->row(item));
        removeDeletedItemFromList(itemText);
    }else {
        if(-1 != tabIndex)
            closeTab(tabIndex);
        delete listWidget->takeItem(listWidget->row(item));
        removeDeletedItemFromList(itemText);
    }

}

void GwMainWindow::removeDeletedItemFromList(const QString &table)
{
    tabList.removeOne(table);
    saveManager();
}

void GwMainWindow::saveManager()
{
    QString dPath = databasePathString();
    QString filePath = dPath + "/" + databasePathMap()["class"] + QString(".srm");

    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly)){
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_7);
    out << databasePathMap();
    out << listOfTabs();

    file.flush();
    file.close();

    return;
}


void GwMainWindow::updateGradeBookTables()
{
    QSqlQuery query(db);
    //!Fetch the reg number,  surname and first name of students in the class
    QString queryStr = ("SELECT reg_num, surname, first_name FROM students");
    if(query.exec(queryStr)){
        if(query.last()){

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
            for(int i = 0; i < listOfTabs().count(); ++i){
                if(listOfTabs()[i] == studentTableName()){
                    continue;
                }
                QString tableN = listOfTabs()[i];
                //! Iterate through the names in each record and store the values
                //! in the variables
                QString regNum = query.value("reg_num").toString();
                QString surname = query.value("surname").toString();
                QString name = query.value("first_name").toString();

                //! Construct a query string
                QString preQuery = QString("INSERT INTO %1(reg_num, surname, first_name) "
                        "VALUES (:reg_num, :surname, :first_name)").arg(tableName(tableN));

                //! create a QSqlQuery object and prepare a query
                QSqlQuery secQuery(db);
                secQuery.prepare(preQuery);
                secQuery.bindValue(":reg_num", regNum);
                secQuery.bindValue(":surname", surname);
                secQuery.bindValue(":first_name", name);
                secQuery.exec();

                refreshGradeBookTable(tableN);
            }
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
        }
    }
}

void GwMainWindow::on_actionSave_triggered()
{
    int index = tabWidget->currentIndex();
    save(tabWidget->tabText(index));
}

bool GwMainWindow::itemModified()
{
    if(model->isDirty()){
        view->setWindowModified(true);
        return true;
    }
    return false;
}


//! This Function is called be emiting QTimer timeout signal but
//! there is pointer error
void GwMainWindow::tabWidgetModified()
{
    GwGradeBookWidget *gradeBook = tabWidgetWidget(tabWidget->currentIndex());
    if(gradeBook && gradeBook->itemModified()){
        addToModifiedTabList(tabWidget->tabText(tabWidget->currentIndex()));
    }else{
        GwStudentView *studentView = dynamic_cast<GwStudentView *>(tabWidget->currentWidget());
        if(studentView && itemModified()){
            addToModifiedTabList(tabWidget->tabText(tabWidget->currentIndex()));
        }
    }
}

void GwMainWindow::addToModifiedTabList(const QString &tabText)
{
    if(!modifiedTabList.contains(tabText, Qt::CaseInsensitive))
        modifiedTabList << tabText;
}

QStringList GwMainWindow::getModifiedTabListz() const
{
    return modifiedTabList;
}

void GwMainWindow::removeFromModifiedTabList(const QString &tabText)
{
    modifiedTabList.removeOne(tabText);
}

bool GwMainWindow::save(const QString &tabText)
{
    int index = indexOfTab(tabText);
    if(-1 != index){
        GwGradeBookWidget *gradeBook = tabWidgetWidget(index);
        if(gradeBook){
            gradeBook->onSave();
            gradeBook->setWindowModified(false);
            removeFromModifiedTabList(tabText);
            return true;
        }else{
            GwStudentView *studentView = dynamic_cast<GwStudentView *>(tabWidget->widget(index));
            if(studentView){
                model->submitAll();
                view->setWindowModified(false);
                removeFromModifiedTabList(tabText);
                return true;
            }
        }
    }
    return false;
}

void GwMainWindow::saveAll()
{
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    for(int c = 0;  c < modifiedTabList.count(); ++c){
        if(!modifiedTabList.isEmpty())
            save(modifiedTabList.at(c));
    }
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
}

bool GwMainWindow::ignore(const QString &tabText)
{
    int index = indexOfTab(tabText);
    if(-1 != index){
        GwGradeBookWidget *gradeBook = tabWidgetWidget(index);
        if(gradeBook){
            gradeBook->onReject();
            removeFromModifiedTabList(tabText);
            return true;
        }else{
            GwStudentView *studentView = dynamic_cast<GwStudentView *>(tabWidget->widget(index));
            if(studentView){
                model->revertAll();
                removeFromModifiedTabList(tabText);
                return true;
            }
        }
    }
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    return false;
}

void GwMainWindow::ignoreAll()
{
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    for(int c = 0; c < modifiedTabList.count(); ++c){
        if(!modifiedTabList.isEmpty())
            ignore(modifiedTabList.at(c));
    }
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
}

void GwMainWindow::print(QPrinter *printer)
{
#ifndef QT_NO_PRINTER
    QTextDocument *doc = createHtmlTableFromModel();
    //printer->setResolution(QPrinter::HighResolution);
    doc->print(printer);
    delete doc;
#endif
}

void GwMainWindow::printDialog()
{
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    if (dialog->exec() == QDialog::Accepted) {
        print(&printer);
    }
}

void GwMainWindow::filePrintPreview()
{
#if QT_CONFIG(printpreviewdialog)
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, &GwMainWindow::print);
    preview.exec();
#endif
}

bool GwMainWindow::okToContinue()
{
    if(isModified()){
        for(int i = 0, j = modifiedTabList.count(); i < j; ++i){
            QString tabText = modifiedTabList.at(i);
            int ret = QMessageBox::question(this, tr("s"), QString(tr("There are unsave changes"
                                                    " in the %1 table.\n"
                                                    "Do you want to save the changes"
                                                    " before closing?")).arg(tabText),
                                  QMessageBox::Save | QMessageBox::SaveAll |
                                  QMessageBox::Ignore | QMessageBox::NoToAll | QMessageBox::Cancel);
            if(ret == QMessageBox::Save){
                save(tabText);
                if(modifiedTabList.count() <= 0){
                    return true;
                }
            }else if (ret == QMessageBox::SaveAll) {
                saveAll();
                return true;
            }else if (ret == QMessageBox::Ignore) {
                ignore(tabText);
                if(modifiedTabList.count() <= 0){
                    return true;
                }
            }else if (ret == QMessageBox::NoToAll) {
                ignoreAll();
                return true;
            }else if (ret == QMessageBox::Cancel) {
                return false;
            }
        }
    }
    return true;
}

bool GwMainWindow::yesToSave(int index)
{
    if(tabWidget->widget(index)->isWindowModified()){
        QString tabText = tabWidget->tabText(index);
        int ret = QMessageBox::question(this, tr("s"), QString(tr("There are unsave changes"
                                                " in the %1 table.\n"
                                                "Do you want to save the changes")).arg(tabText),
                              QMessageBox::Save | QMessageBox::Ignore | QMessageBox::Cancel);
        if(ret == QMessageBox::Save){
            save(tabText);
        }else if(ret == QMessageBox::Ignore){
            ignore(tabText);
        }else if(ret == QMessageBox::Cancel){
            return false;
        }
    }

    return true;
}

void GwMainWindow::on_actionSave_All_triggered()
{
    saveAll();
}

void GwMainWindow::on_actionClose_triggered()
{
    close();
}

void GwMainWindow::on_actionE_xit_triggered()
{
    close();
}


void GwMainWindow::on_actionPrint_triggered()
{
    printDialog();
}

void GwMainWindow::on_actionPrint_Preview_triggered()
{
    filePrintPreview();
}

QTextDocument *GwMainWindow::createHtmlTableFromModel()
{
    GwGradeBookWidget *gradeBook = tabWidgetWidget(tabWidget->currentIndex());

    QTextEdit *textEdit;
    QTableView *tableView;
    if(gradeBook){
        textEdit = gradeBook->textEditor();
        tableView = gradeBook->tableViewer();
    }else{
        textEdit = view->textEditor();
        tableView = view->tableViewer();
    }

    // make a html-dump of table view
    if (tableView) {

        QString stringStream;
        QTextStream out(&stringStream);

        const long rowCount = tableView->model()->rowCount();
        const long columnCount = tableView->model()->columnCount();

        out <<  "<html>\n"
                "<head>\n"
                "<meta Content=\"Text/html; charset=Windows-1251\">\n"
            <<  QString("<title>%1</title>\n").arg(studentTableName())
            <<  "</head>\n"
                "<body bgcolor=#ffffff link=#5000A0>\n"
                "<table border=1 cellspacing=0 cellpadding=2 align=center>\n";

        // headers
        out << textEdit->toHtml();
        out << "<br/>";
        out << "<tr bgcolor=#f0f0f0>";
        for (long column = 0; column < columnCount; column++)
            if (!tableView->isColumnHidden(column))
                out << QString("<th>%1</th>").arg(tableView->model()->headerData(column, Qt::Horizontal).toString());
        out << "</tr>\n";

        // data table
        for (long row = 0; row < rowCount; row++) {
            out << "<tr>";
            for (long column = 0; column < columnCount; column++) {
                if (!tableView->isColumnHidden(column)) {
                    QString data = tableView->model()->data(tableView->model()->index(row, column)).toString().simplified();
                    out << QString("<td bkcolor=0 align=center>%1</td>").arg((!data.isEmpty()) ? data : QString("&nbsp;"));
                }
            }
            out << "</tr>\n";
        }
        out <<  "</table>\n"
            "</body>\n"
            "</html>\n";

        QTextDocument *document = new QTextDocument(this);
        document->setHtml(stringStream);

        return document;
    }

    return new QTextDocument(this);
}

void GwMainWindow::writeSetting()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

void GwMainWindow::readSetting()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(800, 600);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void GwMainWindow::filePrintPdf()
{
#ifndef QT_NO_PRINTER
//! [0]
    QFileDialog fileDialog(this, tr("Export PDF"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
    fileDialog.setDefaultSuffix("pdf");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString fileName = fileDialog.selectedFiles().first();
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    QTextDocument *doc = createHtmlTableFromModel();
    doc->print(&printer);
    delete doc;
    statusBar()->showMessage(tr("Exported \"%1\"")
                             .arg(QDir::toNativeSeparators(fileName)), 3000);
//! [0]
#endif
}


void GwMainWindow::on_actionExport_PDF_triggered()
{
    filePrintPdf();
}

void GwMainWindow::onCreateResultSlip()
{
    GwStudentData studData;
    QModelIndex index = view->tableViewer()->verticalHeader()->currentIndex();
    QSqlRecord record = model->record(index.row());

    studData.setRegNumber(record.value("reg_num").toString());
    studData.setLastName(record.value("surname").toString());
    studData.setFirstName(record.value("first_name").toString());
    studData.setOtherNames(record.value("middle_name").toString());
    studData.setGender(record.value("gender").toString());

    GwResult *resultSlip = new GwResult(db);
    resultSlip->setStudentData(studData);
    resultSlip->setTermInfo(db.databaseName());

    QString level;
    if(resultSlip->getTermInfo().at(2).startsWith("JS", Qt::CaseInsensitive))
        level = "Junior";
    else
        level = "Senior";
    resultSlip->createResultBook(level);

    resultSlip->show();
}

void GwMainWindow::on_actionCreateReportSlip_triggered()
{
    GwResult *resultSlip = new GwResult(db);
    resultSlip->setTermInfo(db.databaseName());
    bool y = resultSlip->createReportSheet();
    if(!y){
        delete resultSlip;
    }else{
        resultSlip->show();
    }
}

void GwMainWindow::on_actionCreateResultSheet_triggered()
{
    GwResultSheet *resultSheet = new GwResultSheet(db);
    resultSheet->setClassDetails(databasePathString());
    resultSheet->createResultSheet();
    resultSheet->show();
}

void GwMainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, tr("About this app"),
                             tr("ResultManager is an application developed to manage "
                                "students' academic performance in a term for secondary"
                                " school students\n\n"
                                "It make use of SQLite database management system"
                                " to keep track of student quiz (tests) and examination"
                                " in a term.\nIt creates results for each of the student"
                                " in a class.\n\n\n"
                                "Company Name:\nGrand World Technology\n\n"
                                "Program Version:\nVersion 1.0"));
}

void GwMainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}
