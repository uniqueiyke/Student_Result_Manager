#ifndef GWASSESSMENTSCOREFORM_H
#define GWASSESSMENTSCOREFORM_H

#include <QDialog>

namespace Ui {
class GwAssessmentScoreForm;
}

class GwAssessmentScoreForm : public QDialog
{
    Q_OBJECT

public:
    explicit GwAssessmentScoreForm(QWidget *parent = nullptr);
    ~GwAssessmentScoreForm();

    QString registrationNumber() const;
    QString lastName() const;
    QString subject() const;
    QString firstQuizScore() const;
    QString secondQuizScore() const;
    QString thirdQuizScore() const;
    QString otherAssQuizScore() const;
    QString examScore() const;
    void setSubjectList(const QStringList &list);

private:
    Ui::GwAssessmentScoreForm *ui;
};

#endif // GWASSESSMENTSCOREFORM_H
