#ifndef GWQUERYDIALOG_H
#define GWQUERYDIALOG_H

#include <QDialog>

namespace Ui {
class GwQueryDialog;
}

class GwQueryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GwQueryDialog(QWidget *parent = nullptr);
    ~GwQueryDialog();

    void addDataToComboBox(const QStringList &tabList);
    QString fieldName();
    QString sortOrder();

private slots:
    void on_sortButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::GwQueryDialog *ui;
};

#endif // GWQUERYDIALOG_H
