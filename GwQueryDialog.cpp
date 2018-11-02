#include "GwQueryDialog.h"
#include "ui_GwQueryDialog.h"

GwQueryDialog::GwQueryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GwQueryDialog)
{
    ui->setupUi(this);
}

GwQueryDialog::~GwQueryDialog()
{
    delete ui;
}

void GwQueryDialog::on_sortButton_clicked()
{
    accept();
}

void GwQueryDialog::on_cancelButton_clicked()
{
    reject();
}

QString GwQueryDialog::fieldName(){
    return ui->columnComboBox->currentText();
}

QString GwQueryDialog::sortOrder(){
    return ui->orderComboBox->currentText();
}

void GwQueryDialog::addDataToComboBox(const QStringList &tabList){

    ui->columnComboBox->addItems(tabList);
}

