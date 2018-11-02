#ifndef GWGRADEBOOKWIDGET_H
#define GWGRADEBOOKWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QTextEdit>
#include <QCloseEvent>


class GwGradeBookWidget : public QWidget
{
    Q_OBJECT
public:
    GwGradeBookWidget(const QSqlDatabase &db, QWidget *parent = nullptr);

    QTableView *tableViewer();
    QSqlTableModel *viewModel();
    QTextEdit *textEditor();
    void setupEditorText(const QString &subject, const QMap<QString, QString> databasePathMap);
    bool itemModified();

signals:
    void modified(bool);

public slots:
    void refreshView(const QString &tableName);
    void sortView(int column, Qt::SortOrder order = Qt::AscendingOrder);
    bool onSave();
    void onReject();

    void calculateTotals();
    void calculateTotalOneRecord();

    void deleteRecord();
    void insertRecord();
    void sortTableCol();

    void insertPositions();
    void setPosition(const QModelIndex &index);
    void onSetPosition();

    void onContextMenuRequested(const QPoint &pos);
    void onHHeaderContextMenuRequested(const QPoint &pos);
    void onVHeaderContextMenuRequested(const QPoint &pos);

private:
    QTableView *tableView;
    QSqlTableModel *model;
    QTextEdit *textEdit;

    void errorMessage();
    bool deletionAlert();
    bool addRecordAlert();
    void setHeaderTitles();
    QString intToOrdinal(int n);
};

#endif // GWGRADEBOOKWIDGET_H
