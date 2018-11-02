#ifndef GWRESULT_H
#define GWRESULT_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QTextCursor>
#include "GwStudentData.h"

QT_BEGIN_NAMESPACE
class QAction;
class QComboBox;
class QFontComboBox;
class QTextEdit;
class QTextCharFormat;
class QMenu;
class QPrinter;
class QTextTable;
QT_END_NAMESPACE

class GwResult : public QMainWindow
{
    Q_OBJECT

    enum {AlignLeft, AlignCenter};
public:
    GwResult(QWidget *parent = nullptr);
    GwResult(const QSqlDatabase &database, QWidget *parent = nullptr);
    ~GwResult();

    void createResultBook(const QString &level);
    bool createReportSheet();
    bool writeFile(const QString &fileName);
    bool readFile(const QString &fileName);
    void setCurrentFileName(const QString &fileName);
    QList<GwGradeBookScores> gradeBookScores();
    QString capitalizeEachWord(const QString &string);

    void setStudentData(const GwStudentData &stuData);
    GwStudentData getStudentData() const;

    void setTermInfo(const QString &dbFilePath);
    QStringList getTermInfo();

private slots:
    void newFile();
    bool saveAs();
    bool save();
    bool open();
    bool okToContinue();
    void documentModified();
    void filePrint();
    void filePrintPreview();
    void filePrintPdf();
    void printPreview(QPrinter *printer);
    void clipboardDataChanged();

    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString &fontFamily);
    void textSize(const QString &p);
    void textStyle(int styleIndex);
    void textColor();
    void textAlign(QAction *alignment);

    void connetToDatabase();
    
    void about();
    void aboutQt();

protected:
    void closeEvent(QCloseEvent *e) override;

private:
    QTextEdit *textEdit;
    QTextCursor textCursor;
    GwStudentData studentData;
    QString curFileName;
    static int databaseCount;
    QSqlDatabase db;
    QString dbConnName;
    QString dbFileName;
    QStringList termInfo;

    QComboBox *listStyleCombo;
    QFontComboBox *textFontCombo;
    QComboBox *textSizeCombo;

    QAction *saveAction;
    QAction *undoAction;
    QAction *redoAction;

    QAction *textBoldAction;
    QAction *textUnderlineAction;
    QAction *textItalicAction;
    QAction *textColorAction;
    QAction *alignLeftAction;
    QAction *alignCenterAction;
    QAction *alignRightAction;
    QAction *alignJustifyAction;

#ifndef QT_NO_CLIPBOARD
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
#endif

    void createStatusBar();
    void createFileMenu();
    void createEditMenu();
    void createToolMenu();
    void createFormatMenu();
    void createHelpMenu();

    bool fetchData();

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment alignment);
    static int getDatabaseCount();
    void setDbConnName();
    QString getDbconnName() const;
    QString setDatabaseConnectionNewName();
    bool setConnection(const QString &conName);
    QString getStudentRegNumber();
    QString dbPathName();
    bool validateRegNumber(const QString &regNum) const;
    void insertAlignedText(QTextTable *table, int row, int col, const QString &text, const QTextCharFormat &format, int alignment);
    void reSetStudentData();
    QStringList getPosition(const QString &regNum);
    void createWindow();
    void writeSetting();
    void readSetting();
    QPair<QString, bool> accronym(const QString &name);
    QString principalComment(double average, QString pos);
    int randomInt(int i);
};

#endif // GWRESULT_H
