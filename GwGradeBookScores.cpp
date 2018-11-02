#include "GwGradeBookScores.h"
#include <QDebug>
#include <QStringList>

GwGradeBookScores::GwGradeBookScores()
{

}

void GwGradeBookScores::setSubjectTitle(const QString &subjectTitle)
{
    g_subjectTitle = subjectTitle.trimmed();
}

QString GwGradeBookScores::subjectTitle() const
{
    return g_subjectTitle;
}

void GwGradeBookScores::setFirstQuizScore(const QString &score)
{
    g_firstQuizScore = score.trimmed();
}

QString GwGradeBookScores::firstQuizScore() const
{
    return g_firstQuizScore;
}

void GwGradeBookScores::setSecondQuizScore(const QString &score)
{
    g_secondQuizScore = score.trimmed();
}

QString GwGradeBookScores::secondQuizScore() const
{
    return g_secondQuizScore;
}

void GwGradeBookScores::setThirdQuizScore(const QString &score)
{
    g_thirdQuizScore = score.trimmed();
}

QString GwGradeBookScores::thirdQuizScore() const
{
    return g_thirdQuizScore;
}

void GwGradeBookScores::setOtherAssessmentsScore(const QString &score)
{
    g_otherAssessmentsScore = score.trimmed();
}

QString GwGradeBookScores::otherAssessmentsScore() const
{
    return g_otherAssessmentsScore;
}

void GwGradeBookScores::setExamScore(const QString &score)
{
    g_examScore = score.trimmed();
}

QString GwGradeBookScores::examScore() const
{
    return g_examScore;
}

QString GwGradeBookScores::calculateTotalQuizScore()
{
    double total = firstQuizScore().toDouble() + secondQuizScore().toDouble()
            + thirdQuizScore().toDouble() + otherAssessmentsScore().toDouble();

    return QString::number(total);
}

QString GwGradeBookScores::calculateTotal()
{
    double total = firstQuizScore().toDouble() + secondQuizScore().toDouble()
            + thirdQuizScore().toDouble() + otherAssessmentsScore().toDouble()
            + examScore().toDouble();
    return QString::number(total);
}

void GwGradeBookScores::setTotalScore(const QString &score)
{
    g_totalScore = score;
}

QString GwGradeBookScores::totalScore() const
{
    return g_totalScore;
}

void GwGradeBookScores::setSubjectPosition(const QString &pos)
{
    g_subjectPosition = pos.trimmed();
}

QString GwGradeBookScores::subJectPosition() const
{
    return g_subjectPosition;
}

void GwGradeBookScores::print()
{
    QStringList list;
    list << subjectTitle() << firstQuizScore() << secondQuizScore() << thirdQuizScore()
         << otherAssessmentsScore() << examScore() << totalScore() << calculateTotal()
         << subJectPosition();
    qDebug() << list ;
}
