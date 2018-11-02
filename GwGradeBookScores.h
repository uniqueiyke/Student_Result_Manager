#ifndef GWGRADEBOOKSCORES_H
#define GWGRADEBOOKSCORES_H

#include <QString>

class GwGradeBookScores
{
public:
    GwGradeBookScores();

    //Public Methods
    void setSubjectTitle(const QString &subjectTitle);
    QString subjectTitle() const;

    void setFirstQuizScore(const QString &score);
    QString firstQuizScore() const;

    void setSecondQuizScore(const QString &score);
    QString secondQuizScore() const;

    void setThirdQuizScore(const QString &score);
    QString thirdQuizScore() const;

    void setOtherAssessmentsScore(const QString &score);
    QString otherAssessmentsScore() const;

    void setExamScore(const QString &score);
    QString examScore() const;

    QString calculateTotalQuizScore();

    QString calculateTotal();
    void setTotalScore(const QString &score);
    QString totalScore() const;

    void setSubjectPosition(const QString &pos);
    QString subJectPosition() const;

    void print();

private:
    QString g_subjectTitle;
    QString g_firstQuizScore;
    QString g_secondQuizScore;
    QString g_thirdQuizScore;
    QString g_otherAssessmentsScore;
    QString g_examScore;
    QString g_quizTotalScore;
    QString g_totalScore;
    QString g_subjectPosition;
};

#endif // GWGRADEBOOKSCORES_H
