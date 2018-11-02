#ifndef GWSTUDENT_H
#define GWSTUDENT_H

#include <QDialog>
#include <QRegularExpressionValidator>

namespace Ui {
class GwStudent;
}

class GwStudent : public QDialog
{
    Q_OBJECT

public:
    explicit GwStudent(QWidget *parent = nullptr);
    ~GwStudent();

    //getter methods
    QString registrationNumber() const;
    QString firstName() const;
    QString lastName() const;
    QString middleName() const;
    QString studentGender() const;
    QRegularExpressionValidator *validation();



private:
    Ui::GwStudent *ui;
};

#endif // GWSTUDENT_H
