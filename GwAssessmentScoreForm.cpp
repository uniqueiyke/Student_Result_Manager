#include "GwAssessmentScoreForm.h"
#include "ui_GwAssessmentScoreForm.h"

#include <QRegularExpressionValidator>
#include <QAbstractItemModel>

GwAssessmentScoreForm::GwAssessmentScoreForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GwAssessmentScoreForm)
{
    ui->setupUi(this);

    auto *validator = new QRegularExpressionValidator(QRegularExpression(QString("20\\d\\d/[Ee][Ss]\\d{4}/[jJSs][Ss]")));
    ui->regNumEdit->setValidator(validator);

    auto *validator2 = new QRegularExpressionValidator(QRegularExpression(QString("[a-zA-Z]+")));
    ui->surnameEdit->setValidator(validator2);
}

GwAssessmentScoreForm::~GwAssessmentScoreForm()
{
    delete ui;
}


QString GwAssessmentScoreForm::registrationNumber() const
{
    return ui->regNumEdit->text().toUpper();
}

QString GwAssessmentScoreForm::lastName() const
{
    return ui->surnameEdit->text().toUpper();
}


QString GwAssessmentScoreForm::subject() const
{
    return ui->subjectCombo->currentText();
}

QString GwAssessmentScoreForm::firstQuizScore() const
{
    return ui->firstQuizSpinBox->text();
}

QString GwAssessmentScoreForm::secondQuizScore() const
{
    return ui->secondQuizSpinBox->text();
}

QString GwAssessmentScoreForm::thirdQuizScore() const
{
    return ui->thirdQuizSpinBox->text();
}

QString GwAssessmentScoreForm::otherAssQuizScore() const
{
    return ui->otherAssSpinBox->text();
}

QString GwAssessmentScoreForm::examScore() const
{
    return ui->examSpinBox->text();
}

void GwAssessmentScoreForm::setSubjectList(const QStringList &list)
{
    ui->subjectCombo->addItems(list);
}


