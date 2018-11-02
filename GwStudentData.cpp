#include "GwStudentData.h"

#include <QRegularExpressionMatch>
#include <QDebug>

GwStudentData::GwStudentData(const QString &regNum, const QString &surname, const QString &firstName)
{
    setRegNumber(regNum);
    setLastName(surname);
    setFirstName(firstName);
}

void GwStudentData::setRegNumber(const QString &regNum)
{
    QRegularExpression regExp(QString("20\\d{2}/[Ee][Ss]\\d{3,4}"));
    QRegularExpressionMatch regMatch = regExp.match(regNum);

    if(regMatch.hasMatch()){
        g_regNum = regNum;
    }
}

QString GwStudentData::regNumber() const
{
    return g_regNum.toUpper();
}

void GwStudentData::setLastName(const QString &surname)
{
    g_lastName = validateName(surname);
}

QString GwStudentData::lastName() const
{
    return g_lastName;
}

void GwStudentData::setFirstName(const QString &firstName)
{
   g_firstName = validateName(firstName);
}

QString GwStudentData::firstName() const
{
    return g_firstName;
}

void GwStudentData::setOtherNames(const QString &otherNames)
{
    g_otherNames = validateName(otherNames);
}

QString GwStudentData::otherNames() const
{
    return g_otherNames;
}

void GwStudentData::setGender(const QString &gender)
{
    QRegExp re(QString("female|male"), Qt::CaseInsensitive);

    if(re.exactMatch(gender)){
        g_gender = gender;
    }
}

QString GwStudentData::gender() const
{
    return g_gender;
}

void GwStudentData::setLevel(const QString &level)
{
    QRegExp re(QString("jss? ?[1-3]|sss? ?[1-3]"), Qt::CaseInsensitive);

    if(re.isValid() && re.exactMatch(level)){
        g_level = level;
    }
}

QString GwStudentData::level() const
{
    return g_level;
}

void GwStudentData::setDateOfBirth(const QDate &dateOfBirth)
{
    g_dateOfBirth = dateOfBirth;
}

void GwStudentData::setDateOfBirth(int day, int month, int year)
{
    g_dateOfBirth = QDate(year, month, day);
}

QDate GwStudentData::dateOfBirth() const
{
    return g_dateOfBirth;
}

QString GwStudentData::dateOfBirthToString() const
{
    return g_dateOfBirth.toString("MMM. d, yyyy");
}

QString GwStudentData::validateName(const QString &name) const
{
    QRegularExpression regExp(QString("[a-zA-Z]+"));
    QRegularExpressionMatch regMatch = regExp.match(name);

    if(!regMatch.hasMatch()){
        return QString();
    }

    return name;
}

QString GwStudentData::studentFullName() const
{
    return lastName() + " " + firstName() + " " + otherNames();
}

void GwStudentData::print() const
{
    qDebug() << regNumber();
    qDebug() << lastName();
    qDebug() << firstName();
    qDebug() << otherNames();
    qDebug() << gender();
    qDebug() << level();
}


