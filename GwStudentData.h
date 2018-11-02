#ifndef GwStudentData_H
#define GwStudentData_H

#include <QString>
#include <QDate>
#include <QList>

#include "GwGradeBookScores.h"

class GwStudentData
{
public:
    GwStudentData() = default;
    GwStudentData(const QString &regNum, const QString &surname, const QString &firstName);

    void setRegNumber(const QString &regNum);
    QString regNumber() const;

    void setLastName(const QString &surname);
    QString lastName() const;

    void setFirstName(const QString &firstName);
    QString firstName() const;

    void setOtherNames(const QString &otherNames);
    QString otherNames() const;

    void setGender(const QString &gender);
    QString gender() const;

    void setLevel(const QString &level);
    QString level() const;

    void setDateOfBirth(const QDate &dateOfBirth);
    void setDateOfBirth(int day, int month, int year);
    QDate dateOfBirth() const;
    QString dateOfBirthToString() const;

    QString validateName(const QString &name) const;
    QString studentFullName() const;
    void print() const;

private:
    QString g_regNum;
    QString g_lastName;
    QString g_firstName;
    QString g_otherNames;
    QString g_gender;
    QString g_level;
    QDate g_dateOfBirth;
};

#endif // GwStudentData_H
