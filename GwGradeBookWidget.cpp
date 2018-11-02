#include "GwGradeBookWidget.h"
#include "GwStudent.h"
#include "GwQueryDialog.h"

#include <QVBoxLayout>
#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>
#include <QHeaderView>
#include <QDebug>
#include <QMenu>
#include <QAction>


GwGradeBookWidget::GwGradeBookWidget(const QSqlDatabase &db, QWidget *parent) : QWidget(parent)
{
    resize(762, 390);

    model = new QSqlTableModel(this, db);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    tableView = new QTableView;
    tableView->setAlternatingRowColors(true);

    textEdit = new QTextEdit;
    textEdit->setFixedHeight(height()/3);
    textEdit->setReadOnly(true);
    textEdit->setStyleSheet(QString("border: none"));

    tableView->setStyleSheet(QString("border: none"));
    tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableView->setModel(model);

    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->horizontalHeader()->setSortIndicatorShown(true);

    auto vlayout = new QVBoxLayout;
    vlayout->addWidget(textEdit);
    vlayout->addWidget(tableView);

    setLayout(vlayout);

    connect(tableView, &QTableView::customContextMenuRequested, this, &GwGradeBookWidget::onContextMenuRequested);
    connect(tableView->horizontalHeader(), &QHeaderView::customContextMenuRequested, this, &GwGradeBookWidget::onHHeaderContextMenuRequested);
    connect(tableView->verticalHeader(), &QHeaderView::customContextMenuRequested, this, &GwGradeBookWidget::onVHeaderContextMenuRequested);
}

QTableView *GwGradeBookWidget::tableViewer()
{
    return tableView;
}

QSqlTableModel *GwGradeBookWidget::viewModel()
{
    return model;
}

QTextEdit *GwGradeBookWidget::textEditor()
{
    return textEdit;
}

void GwGradeBookWidget::refreshView(const QString &tableName)
{
    model->setTable(tableName);
    setHeaderTitles();
    model->select();
}

void GwGradeBookWidget::sortView(int column, Qt::SortOrder order)
{
    model->sort(column, order);
}

void GwGradeBookWidget::setupEditorText(const QString &subject, const QMap<QString, QString> databasePathMap)
{
    QString className = databasePathMap["class"];
    QString term = databasePathMap["term"];
    QString year = databasePathMap["year"];

    QString text = QString(tr("<h2 align=\"center\">EMAK GOD'S OWN SCHOOLS</h2>"
                   "<h1 align=\"center\">%1 GradeBook</h1>"
                   "<h3 align=\"center\"> %2 STUDENTS %3.  %4 TERM</h3>"))
            .arg(subject).arg(className.replace("_", " ")).arg(year.replace("_", "/")).arg(term);

    textEditor()->setHtml(text);
}

bool GwGradeBookWidget::itemModified()
{
    if(model->isDirty()){
        setWindowModified(true);
        return true;
    }

    return false;
}

void GwGradeBookWidget::onReject()
{
    model->revertAll();
}

void GwGradeBookWidget::onContextMenuRequested(const QPoint &pos)
{
    QMenu menu;

    QAction *totalAction = new QAction("Calculate Total", this);
    connect(totalAction, &QAction::triggered, this, &GwGradeBookWidget::calculateTotals);
    menu.addAction(totalAction);

    QAction *insertPositionAction = new QAction("Insert positions", this);
    insertPositionAction->setToolTip(tr("Set position for seleted records base on the order of the rows\n"
                                        "The Records should be sorted in descending "
                                        "order of the Total or Average"));
    connect(insertPositionAction, &QAction::triggered, this, &GwGradeBookWidget::insertPositions);
    menu.addAction(insertPositionAction);

    menu.exec(tableViewer()->viewport()->mapToGlobal(pos));
}

void GwGradeBookWidget::onHHeaderContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);

    QAction *sortAction = new QAction(tr("Sort"));
    sortAction->setToolTip(tr("Sort the table in either ascending or descending order"));
    connect(sortAction, &QAction::triggered, this, &GwGradeBookWidget::sortTableCol);
    menu.addAction(sortAction);

    menu.exec(tableViewer()->horizontalHeader()->viewport()->mapToGlobal(pos));
}

void GwGradeBookWidget::onVHeaderContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    QAction *calcAction = new QAction("Calculate Total", this);
    connect(calcAction, &QAction::triggered, this, &GwGradeBookWidget::calculateTotalOneRecord);
    menu.addAction(calcAction);

    QAction *deleteAction = new QAction("delete row", this);
    connect(deleteAction, &QAction::triggered, this, &GwGradeBookWidget::deleteRecord);
    menu.addAction(deleteAction);

    QAction *addRecordAction = new QAction("add student", this);
    connect(addRecordAction, &QAction::triggered, this, &GwGradeBookWidget::insertRecord);
    menu.addAction(addRecordAction);

    QAction *positionAction = new QAction("Give Postion", this);
    positionAction->setToolTip(tr("Set position base on the order of the rows\n"
                                  "The Records should be sorted in descending "
                                  "order of the Total"));
    connect(positionAction, &QAction::triggered, this, &GwGradeBookWidget::onSetPosition);
    menu.addAction(positionAction);

    menu.exec(tableViewer()->verticalHeader()->viewport()->mapToGlobal(pos));
}

void GwGradeBookWidget::calculateTotals()
{
    QItemSelectionModel *selectionModel = tableViewer()->selectionModel();
    if(selectionModel->hasSelection()){
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
        QModelIndexList selectedModelIndexList = selectionModel->selectedIndexes();
        foreach (QModelIndex modelIndex, selectedModelIndexList) {
            QSqlRecord record = viewModel()->record(modelIndex.row());
            if(!record.isEmpty()){
                //int id = record.value("id").toInt();
                int firstQuiz = record.value("first_quiz").toInt();
                int secondQuiz = record.value("second_quiz").toInt();
                int thirdQuiz = record.value("third_quiz").toInt();
                int otherAss = record.value("other_assessment").toInt();
                int exam = record.value("examination").toInt();

                int total = firstQuiz + secondQuiz + thirdQuiz + otherAss + exam;
                if(0 != total){
                    record.setValue("total", total);
                    bool result = model->setRecord(modelIndex.row(), record);

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
            model->submitAll();
            setWindowModified(false);
            model->select();
        }
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    }
}

void GwGradeBookWidget::calculateTotalOneRecord()
{
    QModelIndex index = tableViewer()->verticalHeader()->currentIndex();
    if(index.isValid()){
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
            QSqlRecord record = viewModel()->record(index.row());
            if(!record.isEmpty()){
                //int id = record.value("id").toInt();
                int firstQuiz = record.value("first_quiz").toInt();
                int secondQuiz = record.value("second_quiz").toInt();
                int thirdQuiz = record.value("third_quiz").toInt();
                int otherAss = record.value("other_assessment").toInt();
                int exam = record.value("examination").toInt();

                int total = firstQuiz + secondQuiz + thirdQuiz + otherAss + exam;
                if(0 != total){
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
            model->submitAll();
            setWindowModified(false);
            model->select();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    }
}

void GwGradeBookWidget::deleteRecord()
{
    QModelIndex index = tableViewer()->currentIndex();
    if(deletionAlert()){
        model->removeRow(index.row());
        viewModel()->submitAll();
        viewModel()->select();
    }
}

void GwGradeBookWidget::insertRecord()
{
    QModelIndex index = tableViewer()->verticalHeader()->currentIndex();
    if(addRecordAlert()){
        GwStudent stu;
        int ret = stu.exec();
        if(QDialog::Accepted != ret){
            return;
        }
        QString regNum = stu.registrationNumber();
        QString surname = stu.lastName();
        QString name = stu.firstName();

        QSqlRecord record = viewModel()->record();
        record.setValue("reg_num", regNum);
        record.setValue("surname", surname);
        record.setValue("first_name", name);
        bool b = model->insertRecord(index.row() + 1, record);
        if(!b){
            QMessageBox::critical(this, tr("Error"), QString(tr("Failed to add\n%1"))
                                  .arg(model->lastError().text()));
            return;
        }
        viewModel()->submitAll();
        viewModel()->select();
    }
}

void GwGradeBookWidget::sortTableCol()
{
    QModelIndex index = tableViewer()->horizontalHeader()->currentIndex();
    int logicalIndex = tableViewer()->horizontalHeader()->logicalIndex(index.column());
    Qt::SortOrder order = tableViewer()->horizontalHeader()->sortIndicatorOrder();
    viewModel()->sort(logicalIndex, order);
    viewModel()->select();
    viewModel()->submitAll();
}

void GwGradeBookWidget::insertPositions()
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

void GwGradeBookWidget::setPosition(const QModelIndex &index)
{
    int row = index.row();
    QSqlRecord record = viewModel()->record(row);
    record.setValue("position", intToOrdinal(row + 1));
    model->setRecord(row, record);
}

void GwGradeBookWidget::onSetPosition()
{
    QModelIndex index = tableViewer()->verticalHeader()->currentIndex();
    setPosition(index);
}
void GwGradeBookWidget::errorMessage()
{
    QMessageBox::critical(this, tr("Error"), tr("No row selected.\n"
                                                "Please select a row to continue"));
}

bool GwGradeBookWidget::onSave()
{
    return model->submitAll();
}

bool GwGradeBookWidget::deletionAlert()
{
    int ret = QMessageBox::question(this, tr("Error"), tr("Deleting students' record from the GradeBook "
                                                "is not adivisable.\nTo maintain consistency of data "
                                                "and records in the ResultManager, it is preferable "
                                                "to delete recodrs from the students' table.\n "
                                                "Do you want to continue with the deletion?"),
                          QMessageBox::Yes | QMessageBox::No);
    if(QMessageBox::No == ret)
        return false;
    return true;
}

bool GwGradeBookWidget::addRecordAlert()
{
    int ret = QMessageBox::question(this, tr("Error"), tr("Adding students' record through the GradeBook "
                                                "is not adivisable.\nTo maintain consistency of data "
                                                "and records in the ResultManager, it is preferable "
                                                "to add recodrs from the students' table.\n "
                                                "Do you want to continue with the operation?"),
                          QMessageBox::Yes | QMessageBox::No);
    if(QMessageBox::No == ret)
        return false;
    return true;
}

void GwGradeBookWidget::setHeaderTitles()
{
    viewModel()->setHeaderData(0, Qt::Horizontal, tr("Reg. Number"));
    viewModel()->setHeaderData(1, Qt::Horizontal, tr("Surname"));
    viewModel()->setHeaderData(2, Qt::Horizontal, tr("First Name"));
    viewModel()->setHeaderData(3, Qt::Horizontal, tr("First Quiz"));
    viewModel()->setHeaderData(4, Qt::Horizontal, tr("Second Quiz"));
    viewModel()->setHeaderData(5, Qt::Horizontal, tr("Third Quiz"));
    viewModel()->setHeaderData(6, Qt::Horizontal, tr("Other Assessment"));
    viewModel()->setHeaderData(7, Qt::Horizontal, tr("Examination"));
    viewModel()->setHeaderData(8, Qt::Horizontal, tr("Total"));
    viewModel()->setHeaderData(9, Qt::Horizontal, tr("Position"));
}

QString GwGradeBookWidget::intToOrdinal(int n)
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

