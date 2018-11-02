#ifndef GWSTUDENTVIEW_H
#define GWSTUDENTVIEW_H

#include <QWidget>
#include <QTableView>
#include <QTextEdit>

namespace Ui {
class GwStudentView;
}

class GwStudentView : public QWidget
{
    Q_OBJECT

public:
    explicit GwStudentView(QWidget *parent = nullptr);
    ~GwStudentView();

    QTextEdit *textEditor();
    QTableView *tableViewer();
private:
    Ui::GwStudentView *ui;
};

#endif // GWSTUDENTVIEW_H
