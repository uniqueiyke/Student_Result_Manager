#include "GwStudentView.h"
#include "ui_GwStudentView.h"

#include <QTextEdit>
#include <QTableView>

GwStudentView::GwStudentView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GwStudentView)
{
    ui->setupUi(this);
    textEditor()->setFixedHeight(height()/3);
}

GwStudentView::~GwStudentView()
{
    delete ui;
}

QTextEdit *GwStudentView::textEditor(){
    return ui->textEdit;
}

QTableView *GwStudentView::tableViewer(){
    return ui->tableView;
}
