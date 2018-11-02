#ifndef GWRESULTSHEET_H
#define GWRESULTSHEET_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QModelIndex>

QT_BEGIN_NAMESPACE
class QTextEdit;
class QTableView;
class QMenu;
class QSqlTableModel;
QT_END_NAMESPACE

class GwResultSheet : public QMainWindow
{
    Q_OBJECT

public:
    GwResultSheet(QWidget *parent = nullptr);
    GwResultSheet(QSqlDatabase database, QWidget *parent = nullptr);
    ~GwResultSheet();

    enum DeletionFlag {SUCCESS = 100, FAILURE = 200, SUSPENDED = 300};

    QTextEdit *textEditor() const;
    QTableView *tableViewer() const;
    QSqlTableModel *viewModel() const;

    void setDatabase(const QString &connectionName);
    bool setConnection();
    bool createConnection();
    static QString setConnectionName();
    static int getConnectionCount();

    void createResultSheet();
    void updateResltSheet();
    void refreshTable(const QString &tableName);

    void writeToFile(const QString &fileName);
    void readFile(const QString &fileName);

    void setClassDetails(const QString &path);
    QMap<QString, QString> getClassDetails() const;

    DeletionFlag deleteTable();

private slots:
void onRefreshTable();
void saveAs();
void save();
void open();

void onCalculateTotals();
void onCalculateTotal();
void onCalculateAverage();
void onCalculateAverages();
void onComputeAll();

void onContextMenuRequested(const QPoint &pos);
void onHHeaderContextMenuRequested(const QPoint &pos);
void onVHeaderContextMenuRequested(const QPoint &pos);

void sortTableCol();
void setPosition(const QModelIndex &index);
void onSetPosition();
void onDeleteTable();

void insertPositions();

private:
    QTextEdit *textEdit;
    QTableView *tableView;
    QSqlTableModel *model;
    QSqlDatabase db;
    QString curFile;
    QStringList fieldNames;
    QMap<QString, QString> classDetails;
    static int connectionCount;
    QString curFileName;

    void createMenus();
    void createStatusBar();
    void headNames();
    QString capitalizeEachWord(const QString &string);
    void calculateAll(const QModelIndex &index);
    void calculateTotalOneRecord(const QModelIndex &index);
    void calculateAverage(const QModelIndex &index);
    QString intToOrdinal(int n);
    QString dbPathName();
    void setupEditorText();
    void setCurrentFileName(const QString &fileName);
};

#endif // GWRESULTSHEET_H
