#include "GwStudent.h"
#include "ui_GwStudent.h"

GwStudent::GwStudent(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GwStudent)
{
    ui->setupUi(this);
    auto *validator = new QRegularExpressionValidator(QRegularExpression(QString("20\\d{2}/[Ee][Ss]\\d{3,4}")));
    ui->regNumEdit->setValidator(validator);
    ui->surnameEdit->setValidator(validation());
    ui->firstNameEdit->setValidator(validation());
    ui->otherNamesEdit->setValidator(validation());
}

GwStudent::~GwStudent()
{
    delete ui;

}

QString GwStudent::registrationNumber() const
{
    return ui->regNumEdit->text().toUpper();
}

QString GwStudent::firstName() const
{
    QString name = ui->firstNameEdit->text();
    name = name.mid(0, 1).toUpper() + name.mid(1).toLower();
    return name;
}

QString GwStudent::lastName() const
{
    QString name = ui->surnameEdit->text();
    name = name.mid(0, 1).toUpper() + name.mid(1).toLower();
    return name;
}

QString GwStudent::middleName() const
{
    QString name = ui->otherNamesEdit->text();
    name = name.mid(0, 1).toUpper() + name.mid(1).toLower();
    return name;
}

QString GwStudent::studentGender() const
{
    if(ui->maleRadioButton->isChecked())
        return "Male";
    else return "Female";
}

QRegularExpressionValidator *GwStudent::validation()
{
    QRegularExpression regExp(QString("[a-zA-Z]+"));

    auto *validator = new QRegularExpressionValidator(regExp, this);
    return validator;
}
