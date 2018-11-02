#ifndef GWCONNECTIONDIALOG_H
#define GWCONNECTIONDIALOG_H

#include <QDialog>
#include <QMap>

namespace Ui {
class GwConnectionDialog;
}

class GwConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GwConnectionDialog(QWidget *parent = nullptr);
    ~GwConnectionDialog();

    QString className();
    QString classStream();
    QString fullClassName();
    QString term();
    QString session();
    QRegularExpression regex();
    QMap<QString, QString> connectionData();
    QString pathString();

private slots:
    //void on_buttonBox_accepted();

private:
    Ui::GwConnectionDialog *ui;
};

#endif // GWCONNECTIONDIALOG_H
