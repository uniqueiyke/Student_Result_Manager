#include "GwResultSheet.h"

#include <QTableView>
#include <QTextEdit>
#include <QtSql>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>
#include <QStatusBar>
#include <QFileDialog>
#include <QStandardItem>
#include <QPair>
#include <QSqlTableModel>
#include <QHeaderView>
#include <QFileInfo>

#ifdef Q_OS_MAC
const QString rsrcPath = ":/mac/images/images/mac";
#else
const QString rsrcPath = ":/win/images/images/win";
#endif

int GwResultSheet::connectionCount = 0;

GwResultSheet::GwResultSheet(QWidget *parent)
    : QMainWindow(parent), db(QSqlDatabase::addDatabase("QSQLITE", setConnectionName()))
{
    setWindowIcon(QIcon(":/images/images/rsh.ico"));

    QWidget *widget = new QWidget;
    widget->setStyleSheet("background-color: green;");

    textEdit = new QTextEdit;
    textEdit->setFixedHeight(widget->height()/3);
    QString wStyleSheet = "background-color: #ffffff; border: none";
    textEdit->setStyleSheet(wStyleSheet);

    model = new QSqlTableModel(this, db);
    tableView = new QTableView;
    tableView->setStyleSheet(wStyleSheet);

    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->horizontalHeader()->setSortIndicatorShown(true);

    QVBoxLayout* vLayout = new QVBoxLayout(widget);
    vLayout->addWidget(textEdit);
    vLayout->addWidget(tableView);

    setCentralWidget(widget);
    resize(1000, 700);

    createMenus();
    createStatusBar();

    setCurrentFileName(QString());

    connect(tableView, &QTableView::customContextMenuRequested, this, &GwResultSheet::onContextMenuRequested);
    connect(tableView->horizontalHeader(), &QHeaderView::customContextMenuRequested, this, &GwResultSheet::onHHeaderContextMenuRequested);
    connect(tableView->verticalHeader(), &QHeaderView::customContextMenuRequested, this, &GwResultSheet::onVHeaderContextMenuRequested);
}

GwResultSheet::GwResultSheet(QSqlDatabase database, QWidget *parent)
    : QMainWindow(parent), db(database)
{
    setWindowIcon(QIcon(":/images/images/rsh.ico"));
    QWidget *widget = new QWidget;
    widget->setStyleSheet("background-color: green");

    textEdit = new QTextEdit;
    textEdit->setFixedHeight(widget->height()/3);
    QString wStyleSheet = "background-color: #ffffff; border: none";
    textEdit->setStyleSheet(wStyleSheet);

    model = new QSqlTableModel(this, db);
    tableView = new QTableView;
    tableView->setStyleSheet(wStyleSheet);

    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->horizontalHeader()->setSortIndicatorShown(true);

    QVBoxLayout* vLayout = new QVBoxLayout(widget);
    vLayout->addWidget(textEdit);
    vLayout->addWidget(tableView);

    setCentralWidget(widget);
    resize(1000, 700);

    createMenus();
    createStatusBar();

    connect(tableView, &QTableView::customContextMenuRequested, this, &GwResultSheet::onContextMenuRequested);
    connect(tableView->horizontalHeader(), &QHeaderView::customContextMenuRequested, this, &GwResultSheet::onHHeaderContextMenuRequested);
    connect(tableView->verticalHeader(), &QHeaderView::customContextMenuRequested, this, &GwResultSheet::onVHeaderContextMenuRequested);
}

GwResultSheet::~GwResultSheet()
{

}

QTextEdit *GwResultSheet::textEditor() const
{
    return textEdit;
}

QTableView *GwResultSheet::tableViewer() const
{
    return tableView;
}

QSqlTableModel *GwResultSheet::viewModel() const
{
    return model;
}

void GwResultSheet::setDatabase(const QString &connectionName)
{
    db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
}

bool GwResultSheet::setConnection()
{
    QString dbpath = dbPathName();
    if(!dbpath.isEmpty()){
        db.setDatabaseName(dbpath);
        if(!db.open()){
            QMessageBox::critical(this, tr("Connection Error"),
                  tr("%1").arg(db.lastError().text()));
            return false;
        }
        return true;
    }

    return false;
}

bool GwResultSheet::createConnection()
{
    if(!db.isValid() || !db.isOpen() || db.databaseName().isEmpty()){
        return setConnection();
    }

    return true;
}

QString GwResultSheet::setConnectionName()
{
    return QString("connetion") + QString::number(getConnectionCount());
}

int GwResultSheet::getConnectionCount()
{
    return connectionCount++;
}

void GwResultSheet::createResultSheet()
{
    if(!createConnection()){
        return;
    }
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    QSqlQuery query(db);
    QSqlQuery secQuery(db);

    QStringList tables = db.tables();
    QString resultSheet = "result_sheet";

    if(tables.contains("students")){
        tables.removeOne("students");
    }

    if(tables.contains(resultSheet)){
        tables.removeOne(resultSheet);
    }

    QString tableList = tables.join(" INT, ");

    QString queryString = QString("CREATE TABLE %1("
                                  "reg_num TEXT UNIQUE NOT NULL,"
                                  "surname VARCHAR(40),"
                                  "first_name VARCHAR(40),"
                                  "%2 INT, total INT,"
                                  "average DECIMAL(4,2),"
                                  "position VARCHAR(10))").arg(resultSheet).arg(tableList);

    bool y = query.exec(queryString);
    if(!y){
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
        QMessageBox::critical(this, tr("Couldent Create Table"), tr("%1").arg(query.lastError().text()));
        onRefreshTable();
        return;
    }

    y = query.exec("SELECT reg_num, surname, first_name FROM students");
    if(!y){
         onRefreshTable();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
        QMessageBox::critical(this, tr("Execution Error"), tr("%1").arg(query.lastError().text()));
        return;
    }


    while (query.next()) {
        //! Iterate through the names in each record and store the values
        //! in the variables
        QString regNum = query.value("reg_num").toString();
        QString surname = query.value("surname").toString();
        QString name = query.value("first_name").toString();

        //! Construct a query string
        QString preQuery = QString("INSERT INTO %1(reg_num, surname, first_name) "
                "VALUES (:reg_num, :surname, :first_name)").arg(resultSheet);

        //! create a QSqlQuery object and prepare a query
        secQuery.prepare(preQuery);
        secQuery.bindValue(":reg_num", regNum);
        secQuery.bindValue(":surname", surname);
        secQuery.bindValue(":first_name", name);
        y = secQuery.exec();
        if(!y){
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
        QMessageBox::critical(this, tr("Execution Error"), tr("%1").arg(query.lastError().text()));
        return;
        }
    }
    onRefreshTable();

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
}

void GwResultSheet::updateResltSheet()
{
    if(!createConnection())
        return;

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    QSqlQuery query(db);
    QSqlQuery secQuery(db);

    QString resultSheet = "result_sheet";

    bool y = query.exec(QString("SELECT * FROM %1").arg(resultSheet));
    if(!y){
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    QMessageBox::critical(this, tr("Error"), tr("%1.\nCreate the ResultSheet table before "
                                                "update the table.").arg(query.lastError().text()));
        return;
    }

    int noOfResultSheetColumns = query.record().count() - 6;

    QStringList dbTables = db.tables();
    int tableCount = dbTables.count();

    if(noOfResultSheetColumns != tableCount - 2){
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
        QMessageBox::critical(this, tr("Can not update"),
                                               tr("There has been changes in the database.\nEither GradeBooks "
                                               "are added or some GradeBooks has been deleted.\n"
                                               "You have to dalete the existing ResultSheet table and "
                                               "and create a new one for the database to be updated.\n"));
        return;
    }

    for(int i = 0; i < tableCount; ++i){
        if("students" == dbTables[i] || resultSheet == dbTables[i])
            continue;
        QString tableColName = "\"" + dbTables[i] + "\"";
        query.exec(QString("SELECT reg_num, total FROM %1").arg(dbTables[i]));
        while (query.next()) {
            QString regNum = "\"" +query.value("reg_num").toString() + "\"";
            QString total = query.value("total").toString().isEmpty() ? "NULL" : query.value("total").toString();
            QString qStr = QString("UPDATE  %1 SET %2 = %3 "
                                   "WHERE reg_num = %4").arg(resultSheet)
                    .arg(tableColName).arg(total).arg(regNum);
            y = secQuery.exec(qStr);
            if(!y){
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
                QMessageBox::critical(this, tr("Could not update Table"), tr("%1").arg(query.lastError().text()));
            }
        }
    }

    onRefreshTable();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
}

void GwResultSheet::refreshTable(const QString &tableName)
{
    model->setTable(tableName);
    headNames();
    bool ok = model->select();
    statusBar()->setStatusTip(QString(tr("ResultSheet reloaded")));
    if(!ok){
        QMessageBox::critical(this, tr("Error"), tr("%1").arg(model->lastError().text()));
    }
}

void GwResultSheet::writeToFile(const QString &fileName)
{
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    QFile file( fileName );
    if( file.open( QIODevice::WriteOnly ) )
    {
        const int rowCount = model->rowCount();
        const int columnCount = model->columnCount();
        QList<QStringList> listOfLists;

        QStringList headerData;
        for (int h=0; h<columnCount; h++){
           headerData << model->headerData(h, Qt::Horizontal).toString();
        }

        for (int i=0; i<rowCount; i++)
        {
            QStringList listOfStr;
            for (int j=0; j<columnCount; j++){
               listOfStr << model->data(model->index(i, j)).toString();
            }
            listOfLists << listOfStr;
        }


        QString textEditText = textEdit->toHtml();
        QDataStream dataStreamOut( &file );
        dataStreamOut.setVersion(QDataStream::Qt_5_9);

        dataStreamOut << textEditText;
        dataStreamOut << rowCount;
        dataStreamOut << columnCount;
        dataStreamOut << headerData;
        dataStreamOut << listOfLists;

        file.flush();
        file.close();
    }
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
}

void GwResultSheet::readFile(const QString &fileName)
{
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream dataStreamIn(&file);
        dataStreamIn.setVersion(QDataStream::Qt_5_9);
        QString textEditText;
        QList<QStringList> listOfLists;
        QStringList headerData;
        int rowCount = 0, columnCount = 0;

        dataStreamIn >> textEditText;
        dataStreamIn >> rowCount;
        dataStreamIn >> columnCount;
        dataStreamIn >> headerData;
        dataStreamIn >> listOfLists;

        file.close();

        QStandardItemModel *itemModel = new QStandardItemModel(rowCount, columnCount);

        for (int h = 0; h < columnCount; h++){
           QVariant data1(headerData.at(h));
           itemModel->setHeaderData(h, Qt::Horizontal, data1);
        }

        for (int i=0; i<rowCount; i++)
        {
            QStringList listOfStr = listOfLists.at(i);
            for (int j=0; j<columnCount; j++){
                QString str = listOfStr.at(j);
                QStandardItem *item = new QStandardItem(str);
                itemModel->setItem(i, j, item);
            }
        }


        QStandardItemModel *sMod = qobject_cast<QStandardItemModel *>(tableView->model());
        if(sMod){
            if(textEdit->document()->isEmpty() && 0 == sMod->columnCount()){
                textEdit->setHtml(textEditText);
                tableViewer()->setModel(itemModel);
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
                return;
            }else{
                GwResultSheet *otherR = new GwResultSheet;
                otherR->textEditor()->setHtml(textEditText);
                otherR->tableViewer()->setModel(itemModel);
                otherR->show();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
                return;
            }
        }else{
            QSqlTableModel *tMod = qobject_cast<QSqlTableModel *>(tableView->model());
            if(tMod){
                if(textEdit->document()->isEmpty() && 0 == tMod->columnCount()){
                    textEdit->setHtml(textEditText);
                    tableViewer()->setModel(itemModel);
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
                    return;
                }else{
                    GwResultSheet *otherR = new GwResultSheet;
                    otherR->textEditor()->setHtml(textEditText);
                    otherR->tableViewer()->setModel(itemModel);
                    otherR->show();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
                    return;
                }
            }
        }

        textEdit->setHtml(textEditText);
        tableViewer()->setModel(itemModel);
    }
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
}

void GwResultSheet::setClassDetails(const QString &path)
{
    QStringList lPath = path.split('/');
    classDetails["year"] = lPath.at(4);
    classDetails["term"] = lPath.at(5).mid(0, 3);
    classDetails["class"] = lPath.at(6);
}

QMap<QString, QString> GwResultSheet::getClassDetails() const
{
    return classDetails;
}

GwResultSheet::DeletionFlag GwResultSheet::deleteTable()
{
    if(!createConnection()){
        return DeletionFlag::FAILURE;
    }

    QSqlQuery query(db);
    QString table = "result_sheet";
    QString queryStr = QString("DROP TABLE %1").arg(table);
    if(db.tables().contains(table, Qt::CaseInsensitive)){
        int ret = QMessageBox::warning(this, tr("Expensive Operation") , tr("Do You want to delete this ResultSheet?\n"
                                      "Deletion of Tables cannot be undo\n"
                                      "Do you want to continue?"),
                             QMessageBox::Yes | QMessageBox::No);
        if(QMessageBox::No == ret)
            return DeletionFlag::SUSPENDED;
        bool r = query.exec(queryStr);
        if(!r){
            QString errMeg = query.lastError().text();
            if(!errMeg.isEmpty()){
                QMessageBox::critical(this, tr("Deletion Error"), tr("%1").arg(errMeg));
            }
            return DeletionFlag::FAILURE;
        }
        model->select();
        return DeletionFlag::SUCCESS;
    }
    return DeletionFlag::FAILURE;
}

void GwResultSheet::onRefreshTable()
{
    if(!createConnection()){
        return;
    }
    tableViewer()->setModel(model);
    setupEditorText();
    refreshTable("result_sheet");
}

void GwResultSheet::saveAs()
{
    QString filePath = "C:/Users/IykeBest/ResultManager/ResiltSheets/";
    QDir fp(filePath);
    if(!fp.exists())
        fp.mkdir(filePath);

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), filePath,
                                                    tr("Result Manager Files (*.rsh)"));

    curFile = fileName;
    writeToFile(fileName);
}

void GwResultSheet::save()
{
    if(curFile.isEmpty()){
        saveAs();
        return;
    }

    writeToFile(curFile);
}

void GwResultSheet::open()
{
    QString filePath = "C:/Users/IykeBest/ResultManager/ResiltSheets/";
    QDir fp(filePath);
    if(!fp.exists())
        fp.mkdir(filePath);

    QString fileName = QFileDialog::getOpenFileName(this, tr("Save File"), filePath,
                                                    tr("Result Manager Files (*.rsh)"));

    readFile(fileName);
}

void GwResultSheet::onCalculateTotals()
{
    QItemSelectionModel *selectionModel = tableViewer()->selectionModel();
    if(selectionModel->hasSelection()){
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
        QModelIndexList selectedModelIndexList = selectionModel->selectedIndexes();
        foreach (QModelIndex modelIndex, selectedModelIndexList) {
            calculateTotalOneRecord(modelIndex);
        }

        model->submitAll();
        setWindowModified(false);
        model->select();

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    }
}

void GwResultSheet::onCalculateTotal()
{
    QModelIndex index = tableViewer()->verticalHeader()->currentIndex();
    calculateTotalOneRecord(index);
    model->submitAll();
    setWindowModified(false);
    model->select();
}

void GwResultSheet::onContextMenuRequested(const QPoint &pos)
{
    QMenu menu;

    QAction *computeAllAction = new QAction("Compute All", this);
    computeAllAction->setToolTip(tr("Calclate Total and Average for each selected row"));
    connect(computeAllAction, &QAction::triggered, this, &GwResultSheet::onComputeAll);
    menu.addAction(computeAllAction);

    QAction *totalAction = new QAction("Calculate Totals", this);
    totalAction->setToolTip(tr("Calclate Total for each selected row"));
    connect(totalAction, &QAction::triggered, this, &GwResultSheet::onCalculateTotals);
    menu.addAction(totalAction);

    QAction *averageAction = new QAction("Calculate Averages", this);
    averageAction->setToolTip(tr("Calclate Average for each selected row"));
    connect(averageAction, &QAction::triggered, this, &GwResultSheet::onCalculateAverages);
    menu.addAction(averageAction);

    QAction *insertPositionAction = new QAction("Insert positions", this);
    insertPositionAction->setToolTip(tr("Set position for seleted records base on the order of the rows\n"
                                        "The Records should be sorted in descending "
                                        "order of the Total or Average"));
    connect(insertPositionAction, &QAction::triggered, this, &GwResultSheet::insertPositions);
    menu.addAction(insertPositionAction);

    menu.exec(tableViewer()->viewport()->mapToGlobal(pos));
}

void GwResultSheet::onHHeaderContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);

    QAction *sortAction = new QAction(tr("Sort"));
    sortAction->setToolTip(tr("Sort the table in either ascending or descending order"));
    connect(sortAction, &QAction::triggered, this, &GwResultSheet::sortTableCol);
    menu.addAction(sortAction);

    menu.exec(tableViewer()->horizontalHeader()->viewport()->mapToGlobal(pos));
}

void GwResultSheet::onVHeaderContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    QAction *calcAction = new QAction("Calculate Total", this);
    calcAction->setToolTip(tr("Calclate Total and the current row only"));
    connect(calcAction, &QAction::triggered, this, &GwResultSheet::onCalculateTotal);
    menu.addAction(calcAction);

    QAction *averageAction = new QAction("Calculate Average", this);
    averageAction->setToolTip(tr("Calclate Average the current row only"));
    connect(averageAction, &QAction::triggered, this, &GwResultSheet::onCalculateAverage);
    menu.addAction(averageAction);

    QAction *positionAction = new QAction("Give Postion", this);
    positionAction->setToolTip(tr("Set position base on the order of the rows\n"
                                  "The Records should be sorted in descending "
                                  "order of the Total or Average"));
    connect(positionAction, &QAction::triggered, this, &GwResultSheet::onSetPosition);
    menu.addAction(positionAction);
    menu.exec(tableViewer()->verticalHeader()->viewport()->mapToGlobal(pos));
}

void GwResultSheet::sortTableCol()
{
    QModelIndex index = tableViewer()->horizontalHeader()->currentIndex();
    int logicalIndex = tableViewer()->horizontalHeader()->logicalIndex(index.column());
    Qt::SortOrder order = tableViewer()->horizontalHeader()->sortIndicatorOrder();
    viewModel()->sort(logicalIndex, order);
    viewModel()->select();
    viewModel()->submitAll();
}

void GwResultSheet::setPosition(const QModelIndex &index)
{
    int row = index.row();
    QSqlRecord record = viewModel()->record(row);
    record.setValue("position", intToOrdinal(row + 1));
    model->setRecord(row, record);
}

void GwResultSheet::onSetPosition()
{
    QModelIndex index = tableViewer()->verticalHeader()->currentIndex();
    setPosition(index);
}

void GwResultSheet::onDeleteTable()
{
    DeletionFlag y = deleteTable();
    if(DeletionFlag::FAILURE == y){
        QMessageBox::critical(this, tr("Delete Operation"), tr("Could not complete the operation.\n"
                                                               "It may be that table is not found in the "
                                                               "database or there is no existing databese"
                                                               " connection."));
    }else if(DeletionFlag::SUCCESS == y){
        QMessageBox::information(this, tr("Delete Operation"), tr("ResultSheet Table Deleted.\n"
                                                                  "Ensure to Recreate ResultSheet table before "
                                                                  "generating student results!!!"));
    }
}

void GwResultSheet::insertPositions()
{
    QItemSelectionModel *selectionModel = tableViewer()->selectionModel();
    if(selectionModel->hasSelection()){
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
        QModelIndexList selectedModelIndexList = selectionModel->selectedIndexes();
        foreach (QModelIndex modelIndex, selectedModelIndexList) {
            setPosition(modelIndex);
        }

        model->submitAll();
        setWindowModified(false);
        model->select();

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    }
}

void GwResultSheet::calculateTotalOneRecord(const QModelIndex &index)
{
    if(index.isValid()){
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
            QSqlRecord record = viewModel()->record(index.row());
            int colCount = model->columnCount();
            if(!record.isEmpty()){
                double total = 0.0;
                for(int i = 3; i < colCount - 3; ++i){
                    if(!record.value(i).isNull()){
                        total += record.value(i).toDouble();
                    }
                }
                if(0.0 != total){
                    record.setValue("total", total);
                    bool result = model->setRecord(index.row(), record);
                    if(!result){
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
                        QMessageBox::critical(this, tr("Error"), QString(tr("Error Ocured\n%1"))
                                              .arg(model->lastError().text()));
                        return;
                    }
                }else{
                    record.setNull("total");
                }
            }
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    }
}

void GwResultSheet::calculateAverage(const QModelIndex &index)
{
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
        if(index.isValid()){
            QSqlRecord record = viewModel()->record(index.row());
            if(!record.isEmpty()){
                int i = record.indexOf("total");
                double total = record.value(i).toDouble();
                int colCount = record.count() - 6;

                if(0.0 != total && 0 != colCount){
                    double average = total / colCount;
                    record.setValue("average", average);
                    bool result = model->setRecord(index.row(), record);
                    if(!result){
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
                        QMessageBox::critical(this, tr("Error"), QString(tr("Error Ocured\n%1"))
                                              .arg(model->lastError().text()));
                        return;
                    }
                }else{
                    record.setNull("average");
                }
            }
        }
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
}

QString GwResultSheet::intToOrdinal(int n)
{
    QString fromInt = QString::number(n);
    if(fromInt.endsWith("11") || fromInt.endsWith("12") || fromInt.endsWith("13"))
        return fromInt + "th";
    int i = fromInt.size() - 1;
    QChar c = fromInt.at(i);
    char ch = c.toLatin1();
    QString ord;
    switch (ch) {
    case '1':
        ord = "st";
        break;
    case '2':
        ord = "nd";
        break;
    case '3':
        ord = "rd";
        break;
    default:
        ord = "th";
        break;
    }

    return fromInt + ord;
}

QString GwResultSheet::dbPathName()
{
    QString dbPath = "C:/Users/IykeBest/ResultManager/";
    QString dbName = QFileDialog::getOpenFileName(this, tr("Save File"), dbPath,
                                                  tr("Database (*.db)"));

    QFileInfo fileInfo(dbName);
    QString fileName = fileInfo.fileName();

    if(fileName.isEmpty())
        return QString();

    setClassDetails(dbName);
    return dbName;
}

void GwResultSheet::setupEditorText()
{
    QString className = getClassDetails()["class"].replace("_", " ");
    QString term = getClassDetails()["term"];
    QString year = getClassDetails()["year"].replace("_", "/");

    QString text = QString(tr("<h2 align=\"center\">EMAK GOD'S OWN SCHOOLS</h2>"
                   "<h1 align=\"center\">%1 ResultSheet</h1>"
                   "<h3 align=\"center\"> %1 STUDENTS %2.  %3 TERM</h3>"))
            .arg(className).arg(year).arg(term);
    textEditor()->setHtml(text);

    setCurrentFileName(QString(className));
}

void GwResultSheet::setCurrentFileName(const QString &fileName)
{
    curFileName = fileName;
    textEdit->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = QFileInfo(fileName).fileName();

    setWindowTitle(tr("%1[*] - %2").arg(shownName, QCoreApplication::applicationName()));
    setWindowModified(false);
}

void GwResultSheet::onCalculateAverage()
{
    QModelIndex index = tableViewer()->verticalHeader()->currentIndex();
    calculateAverage(index);
    model->submitAll();
    setWindowModified(false);
    model->select();
}

void GwResultSheet::onCalculateAverages()
{
    QItemSelectionModel *selectionModel = tableViewer()->selectionModel();
    if(selectionModel->hasSelection()){
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
        QModelIndexList selectedModelIndexList = selectionModel->selectedIndexes();
        foreach (QModelIndex modelIndex, selectedModelIndexList) {
            calculateAverage(modelIndex);
        }

        model->submitAll();
        setWindowModified(false);
        model->select();

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    }
}

void GwResultSheet::onComputeAll()
{
    QItemSelectionModel *selectionModel = tableViewer()->selectionModel();
    if(selectionModel->hasSelection()){
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
        QModelIndexList selectedModelIndexList = selectionModel->selectedIndexes();
        foreach (QModelIndex modelIndex, selectedModelIndexList) {
            calculateAll(modelIndex);
        }

        model->submitAll();
        setWindowModified(false);
        model->select();

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    }
}

void GwResultSheet::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAction = new QAction(tr("&Open"));
    openAction->setShortcut(QKeySequence::Open);
    openAction->setToolTip(tr("Open a file"));
    connect(openAction, &QAction::triggered, this, &GwResultSheet::open);
    fileMenu->addAction(openAction);

    QAction *saveAsAction = new QAction(tr("SaveAs"));
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    saveAsAction->setToolTip(tr("Save as new file"));
    connect(saveAsAction, &QAction::triggered, this, &GwResultSheet::saveAs);
    fileMenu->addAction(saveAsAction);

    QAction *saveAction = new QAction(tr("&Save"));
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setToolTip(tr("Save to file"));
    connect(saveAction, &QAction::triggered, this, &GwResultSheet::save);
    fileMenu->addAction(saveAction);

    QMenu *toolMenu = menuBar()->addMenu(tr("&Tools"));

    QAction *connectionAction = new QAction(tr("Connect to database"));
    connectionAction->setToolTip(tr("connect to database"));
    connect(connectionAction, &QAction::triggered, this, &GwResultSheet::setConnection);
    toolMenu->addAction(connectionAction);

    QAction *resultSheetAction = new QAction(tr("Create ResultSheet"));
    resultSheetAction->setToolTip(tr("Create ResultSheet Table"));
    connect(resultSheetAction, &QAction::triggered, this, &GwResultSheet::createResultSheet);
    toolMenu->addAction(resultSheetAction);

    QAction *displayTableAction = new QAction(tr("Display Table"));
    displayTableAction->setToolTip(tr("Refresh Table"));
    connect(displayTableAction, &QAction::triggered, this, &GwResultSheet::onRefreshTable);
    toolMenu->addAction(displayTableAction);

    QAction *updateAction = new QAction(tr("Update Table"));
    updateAction->setToolTip(tr("Fetch data from database"));
    connect(updateAction, &QAction::triggered, this, &GwResultSheet::updateResltSheet);
    toolMenu->addAction(updateAction);

    QAction *deleteTableAction = new QAction(tr("Delete Table"));
    deleteTableAction->setToolTip(tr("Delete ResultSheet Table from database"));
    connect(deleteTableAction, &QAction::triggered, this, &GwResultSheet::onDeleteTable);
    toolMenu->addAction(deleteTableAction);
}

void GwResultSheet::createStatusBar()
{
    statusBar()->showMessage("Ready");
}

void GwResultSheet::headNames()
{
    QSqlRecord record = model->record();
    int recordCount = record.count();

    for(int r = 0; r < recordCount; ++r){
        model->setHeaderData(r, Qt::Horizontal, capitalizeEachWord(record.fieldName(r)));
    }
}

QString GwResultSheet::capitalizeEachWord(const QString &string)
{
    QString str1 = string;
    QString str = str1.replace("_", " ");
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

void GwResultSheet::calculateAll(const QModelIndex &index)
{
    if(index.isValid()){
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
            QSqlRecord record = viewModel()->record(index.row());
            int colCount = model->columnCount();
            if(!record.isEmpty()){
                double total = 0.0;
                int numOfValidScore = 0;
                for(int i = 3; i < colCount - 3; ++i){
                    if(!record.value(i).isNull()){
                        total += record.value(i).toDouble();
                        numOfValidScore++;
                    }
                }
                if(0.0 != total){
                    double average = total / numOfValidScore;
                    record.setValue("total", total);
                    record.setValue("average", average);
                    bool result = model->setRecord(index.row(), record);
                    if(!result){
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
                        QMessageBox::critical(this, tr("Error"), QString(tr("Error Ocured\n%1"))
                                              .arg(model->lastError().text()));
                        return;
                    }
                }else{
                    record.setNull("total");
                }
            }
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    }
}
