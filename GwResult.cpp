#include "GwResult.h"

#include <QTextEdit>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextFrame>
#include <QTextFormat>
#include <QTextTable>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
#include <QStatusBar>
#include <QMenuBar>
#include <QAction>
#include <QTextStream>
#include <QToolBar>
#include <QClipboard>
#include <QMimeData>
#include <QFontComboBox>
#include <QTextList>
#include <QColorDialog>
#include <QtSql>
#include <QInputDialog>
#include <QDebug>
#include <QDesktopWidget>
#include <QSettings>

#include <ctime>

#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printer)
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#endif
#include <QPrinter>
#if QT_CONFIG(printpreviewdialog)
#include <QPrintPreviewDialog>
#endif
#endif
#endif

#ifdef Q_OS_MAC
const QString rsrcPath = ":/mac/images/images/mac";
#else
const QString rsrcPath = ":/win/images/images/win";
#endif

int GwResult::databaseCount;

GwResult::GwResult(QWidget *parent)
    : QMainWindow(parent)
{
    textEdit = new QTextEdit;
    setCentralWidget(textEdit);
    readSetting();


    setCurrentFileName(QString());
    setWindowIcon(QIcon(":/images/images/res_slip.ico"));
    createFileMenu();
    createEditMenu();
    createToolMenu();
    createFormatMenu();
    createHelpMenu();
    createStatusBar();

    connect(textEdit->document(), &QTextDocument::modificationChanged, this, &GwResult::documentModified);
    connect(textEdit->document(), &QTextDocument::modificationChanged, saveAction, &QAction::setEnabled);
    connect(textEdit->document(), &QTextDocument::undoAvailable, undoAction, &QAction::setEnabled);
    connect(textEdit->document(), &QTextDocument::redoAvailable, redoAction, &QAction::setEnabled);

    undoAction->setEnabled(textEdit->document()->isUndoAvailable());
    redoAction->setEnabled(textEdit->document()->isRedoAvailable());

#ifndef QT_NO_CLIPBOARD
    cutAction->setEnabled(false);
    copyAction->setEnabled(false);

    connect(textEdit, &QTextEdit::copyAvailable, copyAction, &QAction::setEnabled);
    connect(textEdit, &QTextEdit::copyAvailable, cutAction, &QAction::setEnabled);
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &GwResult::clipboardDataChanged);
#endif
}

GwResult::GwResult(const QSqlDatabase &database, QWidget *parent)
    :QMainWindow(parent), db(database)
{
    textEdit = new QTextEdit;
    setCentralWidget(textEdit);
    readSetting();


    setCurrentFileName(QString());
    setWindowIcon(QIcon(":/images/images/res_slip.ico"));
    createFileMenu();
    createEditMenu();
    createToolMenu();
    createFormatMenu();
    createHelpMenu();
    createStatusBar();

    connect(textEdit->document(), &QTextDocument::modificationChanged, this, &GwResult::documentModified);
    connect(textEdit->document(), &QTextDocument::modificationChanged, saveAction, &QAction::setEnabled);
    connect(textEdit->document(), &QTextDocument::undoAvailable, undoAction, &QAction::setEnabled);
    connect(textEdit->document(), &QTextDocument::redoAvailable, redoAction, &QAction::setEnabled);

    undoAction->setEnabled(textEdit->document()->isUndoAvailable());
    redoAction->setEnabled(textEdit->document()->isRedoAvailable());

#ifndef QT_NO_CLIPBOARD
    cutAction->setEnabled(false);
    copyAction->setEnabled(false);

    connect(textEdit, &QTextEdit::copyAvailable, copyAction, &QAction::setEnabled);
    connect(textEdit, &QTextEdit::copyAvailable, cutAction, &QAction::setEnabled);
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &GwResult::clipboardDataChanged);
#endif
}

GwResult::~GwResult()
{

}

void GwResult::createResultBook(const QString &level)
{
    textCursor = textEdit->textCursor();
    textCursor.movePosition(QTextCursor::Start);

    QTextFrame *topFrame = textCursor.currentFrame();
    QTextFrameFormat topFrameFormat = topFrame->frameFormat();
    topFrameFormat.setLeftMargin(10);
    topFrameFormat.setRightMargin(10);
    topFrameFormat.setTopMargin(5);
    topFrameFormat.setBottomMargin(5);
    topFrame->setFrameFormat(topFrameFormat);

    QTextFrameFormat frame0Format;
    frame0Format.setPadding(5);
    textCursor.insertFrame(frame0Format);

    QTextImageFormat imageFormat;
    imageFormat.setName(":/images/images/sch_logo.png");
    imageFormat.setWidth(170);
    imageFormat.setHeight(111);
    textCursor.insertImage(imageFormat, QTextFrameFormat::FloatLeft);

    QFont headerFont("Algerian", 18, QFont::Black);
    headerFont.setCapitalization(QFont::AllUppercase);
    QTextCharFormat boldFormat;
    boldFormat.setForeground(QBrush(Qt::darkGreen));
    boldFormat.setFont(headerFont);

    //This aligns a paragraph to center
    QTextBlockFormat centerBlockFormat;
    centerBlockFormat.setAlignment(Qt::AlignCenter);

    textCursor.insertBlock();
    textCursor.insertBlock();
    textCursor.insertText("Emak god's own secondary school", boldFormat);
    textCursor.insertBlock();
    textCursor.insertText("20 ojike street umuahia, abia state", boldFormat);

    QFont frame1Font("Arial", 12, QFont::Bold);
    frame1Font.setCapitalization(QFont::AllUppercase);
    QTextCharFormat frame1CharFormat;
    frame1CharFormat.setFont(frame1Font);

    QTextCharFormat stuNameFormat;
    stuNameFormat.setFontFamily("Elephant");
    stuNameFormat.setForeground(QBrush(Qt::darkBlue));
    stuNameFormat.setFontCapitalization(QFont::AllUppercase);
    stuNameFormat.setFontItalic(true);
    stuNameFormat.setFontPointSize(12);
    stuNameFormat.setFontWeight(50);

    QTextCharFormat stuRegNFormat = stuNameFormat;
    stuRegNFormat.setForeground(QBrush(QColor("#0076a5")));

    textCursor = topFrame->lastCursorPosition();

    QTextFrameFormat frame1Format;
    textCursor.insertFrame(frame1Format);
    textCursor.insertBlock();
    textCursor.insertText(tr("student's result slip for %1 secondary school").arg(level), frame1CharFormat);
    textCursor.insertBlock();
    textCursor.insertText(tr("Addimission Number:   "), frame1CharFormat);
    textCursor.insertText(tr("%1").arg(studentData.regNumber()), stuRegNFormat);

    textCursor.insertBlock();
    textCursor.insertText(tr("Name:   "), frame1CharFormat);
    textCursor.insertText(tr("%1").arg(studentData.studentFullName()), stuNameFormat);

    textCursor.insertBlock();
    textCursor.insertText(tr("term: %1   code No: ................   "
                          "ED. zone: ................. ").arg(getTermInfo().at(0)), frame1CharFormat);
    textCursor.insertBlock();
    textCursor.insertText(tr("Sex: %1          Class: %2          "
                          "academic year: %3 ").arg(studentData.gender())
                          .arg(getTermInfo().at(2)).arg(getTermInfo().at(1)), frame1CharFormat);

    textCursor = topFrame->lastCursorPosition();
    textCursor.insertBlock();
    textCursor.insertBlock();

    QTextTableFormat tableFormat;
    tableFormat.setAlignment(Qt::AlignCenter);
    tableFormat.setHeaderRowCount(1);
    tableFormat.setCellSpacing(0);
    tableFormat.setBorder(1.5);
    tableFormat.setCellPadding(1);

    QList<GwGradeBookScores> listOfGradeBookRecord = gradeBookScores();
    int listSize = listOfGradeBookRecord.count();

    QTextTable *table = textCursor.insertTable(9, listSize + 2, tableFormat);

    QTextCharFormat tableHeaderRowFormat;
    tableHeaderRowFormat.setFontWeight(QFont::Bold);
    tableHeaderRowFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);

    insertAlignedText(table, 0, 1, tr("M.S.O"), tableHeaderRowFormat, AlignCenter);

    // Set the row headers on the first column
    insertAlignedText(table, 1, 0, tr("Attendance"), tableHeaderRowFormat, AlignLeft);
    insertAlignedText(table, 2, 0, tr("Cont. Ass."), tableHeaderRowFormat, AlignLeft);
    insertAlignedText(table, 3, 0, tr("Exam. Sc."), tableHeaderRowFormat, AlignLeft);
    insertAlignedText(table, 4, 0, tr("Total"), tableHeaderRowFormat, AlignLeft);
    insertAlignedText(table, 5, 0, tr("Weigh. Avg"), tableHeaderRowFormat, AlignLeft);
    insertAlignedText(table, 6, 0, tr("Sub. Pos"), tableHeaderRowFormat, AlignLeft);
    insertAlignedText(table, 7, 0, tr("Teac. Sign"), tableHeaderRowFormat, AlignLeft);

    // set maximum obtainable marks in assessments and exam
    insertAlignedText(table, 2, 1, tr("40"), tableHeaderRowFormat, AlignCenter);
    insertAlignedText(table, 3, 1, tr("60"), tableHeaderRowFormat, AlignCenter);
    insertAlignedText(table, 4, 1, tr("100"), tableHeaderRowFormat, AlignCenter);

    QTextCharFormat subjectTitleFormat;
    QFont subjectTitleFont;
    subjectTitleFont.setItalic(true);
    subjectTitleFont.setBold(true);
    subjectTitleFormat.setFont(subjectTitleFont);

    QTextCharFormat subjectMarksFormat;
    QFont subjectMarksFont;
    subjectMarksFormat.setFont(subjectMarksFont);

    QMap<QString, QString> subTitleMap; //Map object to store abbreviations and meaning

    for(int i = 0, c = 2; i < listSize; ++i, ++c) {
        GwGradeBookScores subjectScores = listOfGradeBookRecord.at(i);
        QPair<QString, bool> accTitlePair = accronym(subjectScores.subjectTitle());
        if(accTitlePair.second){
            insertAlignedText(table, 0, c, tr("%1").arg(accTitlePair.first),  subjectTitleFormat, AlignCenter);
            subTitleMap[accTitlePair.first] = subjectScores.subjectTitle();
        }else {
            insertAlignedText(table, 0, c, tr("%1").arg(accTitlePair.first),  subjectTitleFormat, AlignCenter);
        }
        insertAlignedText(table, 2, c, tr("%1").arg(subjectScores.calculateTotalQuizScore()),  subjectMarksFormat, AlignCenter);
        insertAlignedText(table, 3, c, tr("%1").arg(subjectScores.examScore()),  subjectMarksFormat, AlignCenter);
        insertAlignedText(table, 4, c, tr("%1").arg(subjectScores.totalScore()),  subjectMarksFormat, AlignCenter);
        insertAlignedText(table, 6, c, tr("%1").arg(subjectScores.subJectPosition()),  subjectMarksFormat, AlignCenter);
    }

    QStringList list = getPosition(studentData.regNumber());
    int listCount = list.count();
    QString grandTotal = (0 < listCount ? list.at(0) : "");
    QString grandAverage = (1 < listCount ? list.at(1) : "");
    double avg = 0.0;
    if(!grandAverage.isEmpty()){
        avg = grandAverage.toDouble();
        grandAverage = QString::number(avg, 'f', 2);
    }
    QString grandPosition = (2 < listCount ? list.at(2) : "");
    QString numOfStudents = (3 < listCount ? list.at(3) : "");

    QString palComment = principalComment(avg, grandPosition);

    // Move cursor back to top frame
    textCursor = topFrame->lastCursorPosition();

    QTextCharFormat endCharFormat;
    QFont endCharFont;
    endCharFont.setItalic(true);
    endCharFont.setPointSizeF(14);
    endCharFormat.setFont(endCharFont);

    QTextCharFormat posFormat = endCharFormat;
    posFormat.setForeground(QBrush(QColor("#00b800")));

    QTextCharFormat totalFormat = endCharFormat;
    totalFormat.setForeground(QBrush(QColor("#b93a00")));

    QTextCharFormat sumFormat = endCharFormat;
    sumFormat.setForeground(QBrush(QColor("#700000")));

    textCursor.insertBlock();
    textCursor.insertBlock();
    textCursor.insertText(tr("Total Mark:  "), endCharFormat);
    textCursor.insertText(tr("%1       ").arg(grandTotal), totalFormat);
    textCursor.insertText(tr("Average Mark:  "), endCharFormat);
    textCursor.insertText(tr("%1").arg(grandAverage), totalFormat);
    textCursor.insertBlock();
    textCursor.insertText(tr("Position in class:  "), endCharFormat);
    textCursor.insertText(tr("%1       ").arg(grandPosition), posFormat);
    textCursor.insertText(tr("out of:  "), endCharFormat);
    textCursor.insertText(tr("%1       ").arg(numOfStudents), sumFormat);
    textCursor.insertText(tr("pass/fail: ................. "), endCharFormat);
    textCursor.insertBlock();
    textCursor.insertText(tr("Principal's Comments:  "), endCharFormat);
    textCursor.insertText(tr("%1").arg(palComment), endCharFormat);
    textCursor.insertBlock();
    textCursor.insertText("Principal's Name: ....................................................."
                          "Signature: .................", endCharFormat);
    textCursor.insertBlock();
    textCursor.insertText("Next term begins: ............................................................", endCharFormat);

    QTextCharFormat stickerCharFormat;
    QFont stickerCharFont("Matura MT Script Capitals Regular", 18, QFont::Bold, true);
    stickerCharFont.setCapitalization(QFont::AllUppercase);
    stickerCharFormat.setFont(stickerCharFont);
    stickerCharFormat.setForeground(QBrush(QColor(144, 90, 65)));

    QTextBlockFormat stickerBlockFormat;
    stickerBlockFormat.setAlignment(Qt::AlignCenter);
    textCursor.insertBlock();
    textCursor.insertBlock(stickerBlockFormat);
    textCursor.insertText("Build you child up the way he should go", stickerCharFormat);

    //textCursor.insertBlock();
    textCursor.insertBlock();
    QTextBlockFormat bf;
    bf.setAlignment(Qt::AlignRight);
    QTextCharFormat cf = endCharFormat;
    cf.setFontPointSize(11);
    textCursor.insertBlock(bf);
    textCursor.insertText("Abbreviations and meanings overleaf", cf);

    textCursor.insertBlock();
    textCursor.insertBlock();
    textCursor.insertBlock();
    textCursor.insertBlock();

    QTextBlockFormat keyBlockFormat;
    keyBlockFormat.setAlignment(Qt::AlignLeft);

    textCursor.insertBlock(keyBlockFormat);
    textCursor.insertText("Column abbreviations", endCharFormat);


    //Insert a new frame for column list items
    textCursor.insertFrame(frame1Format);

    QTextCharFormat listCharFormat = endCharFormat;
    listCharFormat.setFontFamily("Times New Roman");
    listCharFormat.setFontPointSize(12);

    textCursor.insertList(QTextListFormat::ListSquare);
    textCursor.insertText(tr("Cont. Ass. --------- Contineous Assessment"), listCharFormat);
    textCursor.insertBlock();
    textCursor.insertText(tr("Exam Sc. --------- Exam Score"));
    textCursor.insertBlock();
    textCursor.insertText(tr("Weigh. Avg. --------- Weighted Average"));
    textCursor.insertBlock();
    textCursor.insertText(tr("Sub. Pos. --------- Subject Position"));
    textCursor.insertBlock();
    textCursor.insertText(tr("Teac. Sign. --------- Teacher's Signature"));

    textCursor = topFrame->lastCursorPosition();
    textCursor.insertBlock();
    textCursor.insertBlock();
    textCursor.insertText("Subject and Row abbreviations", endCharFormat);

    //Insert frame for row list items
    textCursor.insertFrame(frame1Format);

    textCursor.insertList(QTextListFormat::ListSquare);
    textCursor.insertText(tr("M.S.O. -------- Maximum Score Obtainable"), listCharFormat);

    QTextList *textList = textCursor.insertList(QTextListFormat::ListCircle);
    QList<QString> mapKeys = subTitleMap.keys();
    for(int k = 0, j = mapKeys.size(); k < j; ++k){
        QString key = mapKeys.at(k);
        textCursor.insertText(tr("%1. --------- %2").arg(key).arg(subTitleMap[key]), listCharFormat);
        textCursor.insertBlock();
    }

    textList->remove(textList->item(textList->count() - 1));

    reSetStudentData();
}

bool GwResult::writeFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly)){
        QMessageBox::critical(this, tr("File saving error"), tr("Could not save file\n"
                                                                "Error occur in an attempt to save"
                                                                "the file."));
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_9);
    out << textEdit->toHtml();

    file.close();
    file.flush();

    return true;
}

bool GwResult::readFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly)){
        QMessageBox::critical(this, tr("Open file error"), tr("Could not open file\n"
                                                                "Error occur in an attempt to open"
                                                                "the file."));
        return false;
    }

    QString str;
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_9);
    in >> str;

    file.close();

    if(!textEdit->toPlainText().isEmpty()){
        QPoint h = pos() + QPoint(30, 30);;

        auto *newWindow = new GwResult;
        newWindow->move(h);
        newWindow->textEdit->setHtml(str);
        newWindow->show();
        newWindow->setWindowModified(false);

        return true;
    }
    textEdit->setHtml(str);
    setWindowModified(false);
    return true;
}

void GwResult::connetToDatabase()
{
    setDbConnName();
    setConnection(getDbconnName());
}

void GwResult::about()
{
    QMessageBox::information(this, tr("About this app"),
                             tr("This application create a result slip for each "
                                "student in a class at the end of the term\n"
                                "The students' perfomance in a term is managed by the "
                                "ResultManager using SQLite database management system"
                                " to keep track of student quiz (tests) and examination"
                                " in a term.\n\n\n"
                                "Company Name:\nGrand World Technology\n\n"
                                "Program Version:\nVersion 1.0"));
}

void GwResult::aboutQt()
{
    QApplication::aboutQt();
}

void GwResult::setCurrentFileName(const QString &fileName)
{
    curFileName = fileName;
    textEdit->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = QFileInfo(fileName).fileName();

    setWindowTitle(tr("%1[*] - %2").arg(shownName, QCoreApplication::applicationName()));
    setWindowModified(false);
}

QList<GwGradeBookScores> GwResult::gradeBookScores()
{
    QSqlQuery query(db);
    QList<QString> dbTables = db.tables();
    int numberOfTables = dbTables.count();

    QList<GwGradeBookScores> subjectScoresList;

    for(int i = 0; i < numberOfTables; ++i){
        QString subject = dbTables.at(i);
        if("students" == subject || "result_sheet" == subject)
            continue;
        QString queryStr = QString("SELECT * FROM %1 WHERE reg_num = %2")
                .arg(subject).arg("\"" + studentData.regNumber() + "\"");
        if(query.exec(queryStr)){
            GwGradeBookScores subjectScores;
            while (query.next()) {
                subjectScores.setSubjectTitle(capitalizeEachWord(subject.replace("_", " ")));
                subjectScores.setFirstQuizScore(query.value("first_quiz").toString());
                subjectScores.setSecondQuizScore(query.value("second_quiz").toString());
                subjectScores.setThirdQuizScore(query.value("third_quiz").toString());
                subjectScores.setOtherAssessmentsScore(query.value("other_assessment").toString());
                subjectScores.setExamScore(query.value("examination").toString());
                subjectScores.setTotalScore(query.value("total").toString());
                subjectScores.setSubjectPosition(query.value("position").toString());
            }
            subjectScoresList.append(subjectScores);
        }else{
            QMessageBox::critical(nullptr, QObject::tr("Error"),
                                 tr("%1").arg(query.lastError().text()));
        }
    }

    return subjectScoresList;
}

QString GwResult::capitalizeEachWord(const QString &string)
{
    QString str = string;
    QStringList strList = str.split(" ");
    for(int i = 0, c = strList.count(); i < c; ++i) {
        QString s = strList.at(i);
        QString u = s.mid(0,1).toUpper();
        QString l = s.mid(1).toLower();
        QString ul = u + l;
        strList.replace(i, ul);
    }

    return strList.join(" ");
}

GwStudentData GwResult::getStudentData() const
{
    return studentData;
}

void GwResult::setTermInfo(const QString &dbFilePath)
{
    QFileInfo fileinfo(dbFilePath);
    if(fileinfo.exists() && "db" == fileinfo.suffix()){
        QStringList list = dbFilePath.split("/");
        int y = list.indexOf(QRegularExpression("\\d{4}_\\d{4}", QRegularExpression::CaseInsensitiveOption));
        int t = list.indexOf(QRegularExpression("\\d[snr][td]term", QRegularExpression::CaseInsensitiveOption));
        QString year = list.at(y);
        QString term = list.at(t);

        QStringList l;
        l << term.remove(QRegularExpression("T\\w+"));
        l << year.replace("_", "/");
        l << fileinfo.fileName().replace("_", "").replace(".db", "");
        termInfo = l;
    }
}

QStringList GwResult::getTermInfo()
{
    return termInfo;
}


void GwResult::setStudentData(const GwStudentData &stuData)
{
    studentData = stuData;
}

void GwResult::newFile()
{
    QPoint h = pos() + QPoint(30, 30);;

    auto *newWindow = new GwResult;
    newWindow->move(h);
    newWindow->show();
}

void GwResult::createStatusBar()
{
    statusBar()->showMessage("Ready");
}

void GwResult::createFileMenu()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *toolBar = addToolBar(tr("file actions"));

    const QIcon newIcon = QIcon::fromTheme("document-open", QIcon(rsrcPath + "/filenew.png"));
    QAction *newAction = new QAction(newIcon, tr("&New"));
    newAction->setShortcut(QKeySequence::Open);
    newAction->setToolTip(tr("new file"));
    connect(newAction, &QAction::triggered, this, &GwResult::newFile);
    fileMenu->addAction(newAction);
    toolBar->addAction(newAction);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(rsrcPath + "/fileopen.png"));
    QAction *openAction = new QAction(openIcon, tr("&Open"));
    openAction->setShortcut(QKeySequence::Open);
    openAction->setToolTip(tr("Open a file"));
    connect(openAction, &QAction::triggered, this, &GwResult::open);
    fileMenu->addAction(openAction);
    toolBar->addAction(openAction);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(rsrcPath + "/filesave.png"));
    saveAction = new QAction(saveIcon, tr("&Save"));
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setToolTip(tr("Save to file"));
    saveAction->setEnabled(false);
    connect(saveAction, &QAction::triggered, this, &GwResult::save);
    fileMenu->addAction(saveAction);
    toolBar->addAction(saveAction);

    QAction *saveAsAction = new QAction(tr("SaveAs"));
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    saveAsAction->setToolTip(tr("Save as new file"));
    connect(saveAsAction, &QAction::triggered, this, &GwResult::saveAs);
    fileMenu->addAction(saveAsAction);

#ifndef QT_NO_PRINTER
    const QIcon printIcon = QIcon::fromTheme("document-print", QIcon(rsrcPath + "/fileprint.png"));
    QAction *printAction = new QAction(printIcon, tr("&Print..."));
    printAction->setShortcut(QKeySequence::Print);
    connect(printAction, &QAction::triggered, this, &GwResult::filePrint);
    fileMenu->addAction(printAction);
    toolBar->addAction(printAction);

    const QIcon filePrintIcon = QIcon::fromTheme("fileprint", QIcon(rsrcPath + "/fileprint.png"));
    QAction *printPreviewAction = new QAction(filePrintIcon, tr("Print Preview..."));
    connect(printPreviewAction, &QAction::triggered, this, &GwResult::filePrintPreview);
    fileMenu->addAction(printPreviewAction);

    const QIcon exportPdfIcon = QIcon::fromTheme("exportpdf", QIcon(rsrcPath + "/exportpdf.png"));
    QAction *exportPdfAction = new QAction(exportPdfIcon, tr("&Export PDF..."));
    exportPdfAction->setShortcut(Qt::CTRL + Qt::Key_D);
    connect(exportPdfAction, &QAction::triggered, this, &GwResult::filePrintPdf);
    fileMenu->addAction(exportPdfAction);
    toolBar->addAction(exportPdfAction);

    fileMenu->addSeparator();
#endif

    QAction *closeAction = new QAction(tr("Close"));
    closeAction->setShortcut(tr("Ctrl+W"));
    connect(closeAction, &QAction::triggered, this, &GwResult::close);
    fileMenu->addAction(closeAction);

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAction = new QAction(exitIcon, tr("E&xit"));
    exitAction->setShortcuts(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, qApp, &QApplication::closeAllWindows);
    exitAction->setStatusTip(tr("Exit the application"));
    fileMenu->addAction(exitAction);
}

void GwResult::createEditMenu()
{
    QToolBar *toolBar = addToolBar(tr("Edit Actions"));
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon(rsrcPath + "/editundo.png"));
    undoAction = editMenu->addAction(undoIcon, tr("&Undo"), textEdit, &QTextEdit::undo);
    undoAction->setShortcut(QKeySequence::Undo);
    toolBar->addAction(undoAction);

    const QIcon redoIcon = QIcon::fromTheme("edit-redo", QIcon(rsrcPath + "/editredo.png"));
    redoAction = editMenu->addAction(redoIcon, tr("&Redo"), textEdit, &QTextEdit::redo);
    redoAction->setPriority(QAction::LowPriority);
    redoAction->setShortcut(QKeySequence::Redo);
    toolBar->addAction(redoAction);
    editMenu->addSeparator();

#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(rsrcPath + "/editcut.png"));
    cutAction = editMenu->addAction(cutIcon, tr("Cu&t"), textEdit, &QTextEdit::cut);
    cutAction->setPriority(QAction::LowPriority);
    cutAction->setShortcut(QKeySequence::Cut);
    toolBar->addAction(cutAction);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(rsrcPath + "/editcopy.png"));
    copyAction = editMenu->addAction(copyIcon, tr("&Copy"), textEdit, &QTextEdit::copy);
    copyAction->setPriority(QAction::LowPriority);
    copyAction->setShortcut(QKeySequence::Copy);
    toolBar->addAction(copyAction);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(rsrcPath + "/editpaste.png"));
    pasteAction = editMenu->addAction(pasteIcon, tr("&Paste"), textEdit, &QTextEdit::paste);
    pasteAction->setPriority(QAction::LowPriority);
    pasteAction->setShortcut(QKeySequence::Paste);
    toolBar->addAction(pasteAction);
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        pasteAction->setEnabled(md->hasText());
#endif
    editMenu->addSeparator();

    QAction *selectAllAction = editMenu->addAction(tr("Select All"), textEdit, &QTextEdit::selectAll);
    selectAllAction->setShortcut(QKeySequence::SelectAll);
}

void GwResult::createToolMenu()
{
    QMenu *toolMenu = menuBar()->addMenu(tr("&Tools"));
    
    QAction *reportSheetAction = new QAction(tr("Create Student Report"));
    reportSheetAction->setToolTip(tr("Create Report Sheet "));
    connect(reportSheetAction, &QAction::triggered, this, &GwResult::createWindow);
    toolMenu->addAction(reportSheetAction);

    QAction *connectToDBAction = new QAction(tr("Connect to Database"));
    connectToDBAction->setToolTip(tr("Set Connetion"));
    connect(connectToDBAction, &QAction::triggered, this, &GwResult::connetToDatabase);
    toolMenu->addAction(connectToDBAction);
}

void GwResult::createFormatMenu()
{
    QToolBar *toolBar = addToolBar(tr("Format Actions"));
    QMenu *menu = menuBar()->addMenu(tr("F&ormat"));

    const QIcon boldIcon = QIcon::fromTheme("format-text-bold", QIcon(rsrcPath + "/textbold.png"));
    textBoldAction = menu->addAction(boldIcon, tr("&Bold"), this, &GwResult::textBold);
    textBoldAction->setShortcut(Qt::CTRL + Qt::Key_B);
    textBoldAction->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    textBoldAction->setFont(bold);
    toolBar->addAction(textBoldAction);
    textBoldAction->setCheckable(true);

    const QIcon italicIcon = QIcon::fromTheme("format-text-italic", QIcon(rsrcPath + "/textitalic.png"));
    textItalicAction = menu->addAction(italicIcon, tr("&Italic"), this, &GwResult::textItalic);
    textItalicAction->setPriority(QAction::LowPriority);
    textItalicAction->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    textItalicAction->setFont(italic);
    toolBar->addAction(textItalicAction);
    textItalicAction->setCheckable(true);

    const QIcon underlineIcon = QIcon::fromTheme("format-text-underline", QIcon(rsrcPath + "/textunder.png"));
    textUnderlineAction = menu->addAction(underlineIcon, tr("&Underline"), this, &GwResult::textUnderline);
    textUnderlineAction->setShortcut(Qt::CTRL + Qt::Key_U);
    textUnderlineAction->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    textUnderlineAction->setFont(underline);
    toolBar->addAction(textUnderlineAction);
    textUnderlineAction->setCheckable(true);

    menu->addSeparator();

    const QIcon leftIcon = QIcon::fromTheme("format-justify-left", QIcon(rsrcPath + "/textleft.png"));
    alignLeftAction = new QAction(leftIcon, tr("&Left"), this);
    alignLeftAction->setShortcut(Qt::CTRL + Qt::Key_L);
    alignLeftAction->setCheckable(true);
    alignLeftAction->setPriority(QAction::LowPriority);

    const QIcon centerIcon = QIcon::fromTheme("format-justify-center", QIcon(rsrcPath + "/textcenter.png"));
    alignCenterAction = new QAction(centerIcon, tr("C&enter"), this);
    alignCenterAction->setShortcut(Qt::CTRL + Qt::Key_E);
    alignCenterAction->setCheckable(true);
    alignCenterAction->setPriority(QAction::LowPriority);

    const QIcon rightIcon = QIcon::fromTheme("format-justify-right", QIcon(rsrcPath + "/textright.png"));
    alignRightAction = new QAction(rightIcon, tr("&Right"), this);
    alignRightAction->setShortcut(Qt::CTRL + Qt::Key_R);
    alignRightAction->setCheckable(true);
    alignRightAction->setPriority(QAction::LowPriority);

    const QIcon fillIcon = QIcon::fromTheme("format-justify-fill", QIcon(rsrcPath + "/textjustify.png"));
    alignJustifyAction = new QAction(fillIcon, tr("&Justify"), this);
    alignJustifyAction->setShortcut(Qt::CTRL + Qt::Key_J);
    alignJustifyAction->setCheckable(true);
    alignJustifyAction->setPriority(QAction::LowPriority);

    // Make sure the alignLeft  is always left of the alignRight
    QActionGroup *alignGroup = new QActionGroup(this);
    connect(alignGroup, &QActionGroup::triggered, this, &GwResult::textAlign);

    if (QApplication::isLeftToRight()) {
        alignGroup->addAction(alignLeftAction);
        alignGroup->addAction(alignCenterAction);
        alignGroup->addAction(alignRightAction);
    } else {
        alignGroup->addAction(alignRightAction);
        alignGroup->addAction(alignCenterAction);
        alignGroup->addAction(alignLeftAction);
    }
    alignGroup->addAction(alignJustifyAction);

    toolBar->addActions(alignGroup->actions());
    menu->addActions(alignGroup->actions());

    menu->addSeparator();

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    textColorAction = menu->addAction(pix, tr("&Color..."), this, &GwResult::textColor);
    toolBar->addAction(textColorAction);

    toolBar = addToolBar(tr("Format Actions"));
    toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(toolBar);

    listStyleCombo = new QComboBox(toolBar);
    toolBar->addWidget(listStyleCombo);
    listStyleCombo->addItem("Standard");
    listStyleCombo->addItem("Bullet List (Disc)");
    listStyleCombo->addItem("Bullet List (Circle)");
    listStyleCombo->addItem("Bullet List (Square)");
    listStyleCombo->addItem("Ordered List (Decimal)");
    listStyleCombo->addItem("Ordered List (Alpha lower)");
    listStyleCombo->addItem("Ordered List (Alpha upper)");
    listStyleCombo->addItem("Ordered List (Roman lower)");
    listStyleCombo->addItem("Ordered List (Roman upper)");

    connect(listStyleCombo, QOverload<int>::of(&QComboBox::activated), this, &GwResult::textStyle);

    textFontCombo = new QFontComboBox(toolBar);
    toolBar->addWidget(textFontCombo);
    connect(textFontCombo, QOverload<const QString &>::of(&QComboBox::activated), this, &GwResult::textFamily);

    textSizeCombo = new QComboBox(toolBar);
    textSizeCombo->setObjectName("textSizeCombo");
    toolBar->addWidget(textSizeCombo);
    textSizeCombo->setEditable(true);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    foreach (int size, standardSizes)
        textSizeCombo->addItem(QString::number(size));
    textSizeCombo->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(textSizeCombo, QOverload<const QString &>::of(&QComboBox::activated), this, &GwResult::textSize);
}

void GwResult::createHelpMenu()
{
    QMenu *helpMenu = menuBar()->addMenu(tr("&About"));
    
    QAction *aboutAction = new QAction(tr("About"));
    aboutAction->setToolTip(tr("About this application"));
    connect(aboutAction, &QAction::triggered, this, &GwResult::about);
    helpMenu->addAction(aboutAction);

    QAction *aboutQtAction = new QAction(tr("About Qt"));
    aboutQtAction->setToolTip(tr("About this application"));
    connect(aboutQtAction, &QAction::triggered, this, &GwResult::aboutQt);
    helpMenu->addAction(aboutQtAction);
}

bool GwResult::fetchData()
{
    if(!db.isValid() || !db.isOpen()){
        connetToDatabase();
    }

    QString regNum = getStudentRegNumber();
    if(regNum.isEmpty()){
        return false;
    }

    QSqlQuery query(db);
    QString queryStr = QString("SELECT * FROM students WHERE reg_num = %1")
            .arg("\"" + regNum + "\"");
    if(query.exec(queryStr)){
        while (query.next()) {
            studentData.setRegNumber(query.value("reg_num").toString());
            studentData.setLastName(query.value("surname").toString());
            studentData.setFirstName(query.value("first_name").toString());
            studentData.setOtherNames(query.value("middle_name").toString());
            studentData.setGender(query.value("gender").toString());
        }
    }else {
        QMessageBox::critical(this, QObject::tr("Error"),
                             tr("%1").arg(query.lastError().text()));
        return false;
    }

    return true;
}

bool GwResult::createReportSheet()
{
    if(!fetchData())
        return false;

    if(studentData.lastName().isEmpty()){
        QMessageBox::critical(this, QObject::tr("Invalid Reg. Number"),
                             tr("There is no student with such number in this class.\n"
                                "Check the registration numbers and enter a valid "
                                "student reg. number.\nThank You"));
        return false;
    }
    QString level;
    if(getTermInfo().at(2).startsWith("JS", Qt::CaseInsensitive))
        level = "Junior";
    else
        level = "Senior";
    createResultBook(level);
    return true;
}

void GwResult::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void GwResult::fontChanged(const QFont &font)
{
    textFontCombo->setCurrentIndex(textFontCombo->findText(QFontInfo(font).family()));
    textSizeCombo->setCurrentIndex(textSizeCombo->findText(QString::number(font.pointSize())));
    textBoldAction->setChecked(font.bold());
    textItalicAction->setChecked(font.italic());
    textUnderlineAction->setChecked(font.underline());
}

void GwResult::colorChanged(const QColor &color)
{
    QPixmap pixmap(16, 16);
    pixmap.fill(color);
    textColorAction->setIcon(pixmap);
}

void GwResult::alignmentChanged(Qt::Alignment alignment)
{
    if (alignment & Qt::AlignLeft)
        alignLeftAction->setChecked(true);
    else if (alignment & Qt::AlignHCenter)
        alignCenterAction->setChecked(true);
    else if (alignment & Qt::AlignRight)
        alignRightAction->setChecked(true);
    else if (alignment & Qt::AlignJustify)
        alignJustifyAction->setChecked(true);
}

int GwResult::getDatabaseCount()
{
    return ++databaseCount;
}

void GwResult::setDbConnName()
{
    dbConnName = "connection_name" + setDatabaseConnectionNewName();
}

QString GwResult::getDbconnName() const
{
    return dbConnName;
}

QString GwResult::setDatabaseConnectionNewName()
{
    return (getDbconnName() + QString::number(getDatabaseCount()));
}

bool GwResult::setConnection(const QString &conName)
{
    QString dbName = dbPathName();
    if(!db.isValid()){
        if(QSqlDatabase::contains(conName)){
            db = QSqlDatabase::database(conName);
            db.setDatabaseName(dbName);
        }else{
            db = QSqlDatabase::addDatabase("QSQLITE", conName);
            db.setDatabaseName(dbName);
        }
    }else if (db.databaseName() == dbName && QSqlDatabase::contains(conName)){
        if(!db.isOpen())
            if (!db.open()) {
                QMessageBox::critical(nullptr, QObject::tr("Database Error"),
                                     tr("%1").arg(db.lastError().text()));
                return false;
            }
        return true;
    }else if (db.databaseName() != dbName && db.connectionName() == conName) {
        db.setDatabaseName(dbName);
    } else if (db.databaseName() != dbName && db.connectionName() != conName && QSqlDatabase::contains(conName)) {
        db = QSqlDatabase::database(conName);
        db.setDatabaseName(dbName);
    }else{
        db = QSqlDatabase::addDatabase("QSQLITE", conName);
        db.setDatabaseName(dbName);
    }
    if (!db.open()) {
        QMessageBox::critical(nullptr, QObject::tr("Database Error"),
                             tr("%1").arg(db.lastError().text()));
        return false;
    }
    return true;
}

QString GwResult::getStudentRegNumber()
{
    QInputDialog inputDlg(this);
    inputDlg.setLabelText(tr("Enter the Student Reg. Number"));
    inputDlg.setOkButtonText(tr("Get Data"));
    inputDlg.setCancelButtonText("Cancel");
    int ret = inputDlg.exec();
    if(ret == QDialog::Rejected)
        return QString();
    if(!validateRegNumber(inputDlg.textValue())){
        QMessageBox::StandardButton ret = QMessageBox::critical(this, QObject::tr("Invalid Reg. Number"),
                             tr("The value '%1' entered is an "
                                "invalid registration number\n"
                                "Do you want re-enter the number").arg(inputDlg.textValue()),
                                QMessageBox::Yes | QMessageBox::No);
        if(ret == QMessageBox::No)
            return QString();
        else
            return getStudentRegNumber();
    }

    return inputDlg.textValue().toUpper().trimmed();
}

QString GwResult::dbPathName()
{
    QString dbPath = "C:/Users/IykeBest/ResultManager/";
    QString dbName = QFileDialog::getOpenFileName(this, tr("Save File"), dbPath,
                                                  tr("Database (*.db)"));
    setTermInfo(dbName);

    return dbName;
}

bool GwResult::validateRegNumber(const QString &regNum) const
{
    QRegularExpression regExp(QString("20\\d{2}/[Ee][Ss]\\d{3,4}"));
    QRegularExpressionMatch regMatch = regExp.match(regNum);

    if(!regMatch.hasMatch()){
         return false;
    }
    return true;
}

void GwResult::insertAlignedText(QTextTable *table, int row, int col, const QString &text, const QTextCharFormat &format, int alignment)
{
    QTextTableCellFormat tableCellFormat;
    tableCellFormat.setTopPadding(8);
    tableCellFormat.setBottomPadding(8);

    QTextTableCell tableCell = table->cellAt(row, col);
    tableCell.setFormat(tableCellFormat);

    textCursor = tableCell.lastCursorPosition();
    QTextBlockFormat celFormat = textCursor.blockFormat();
    if(alignment == AlignCenter){
        celFormat.setAlignment(Qt::AlignCenter);
    }else {
        celFormat.setAlignment(Qt::AlignLeft);
    }
    textCursor.setBlockFormat(celFormat);

    textCursor.insertText(text, format);
}

void GwResult::reSetStudentData()
{
    studentData.setRegNumber(QString());
    studentData.setLastName(QString());
    studentData.setFirstName(QString());
    studentData.setOtherNames(QString());
    studentData.setGender(QString());
    studentData.setLevel(QString());
    studentData.setDateOfBirth(QDate());
}

QStringList GwResult::getPosition(const QString &regNum)
{
    QStringList list;
    QSqlQuery query(db);
    QString queryStr = QString("SELECT total, average, position FROM result_sheet WHERE reg_num = %1")
            .arg("\"" + regNum + "\"");
    if(query.exec(queryStr)){
        while (query.next()) {
            list.append(query.value("total").toString());
            list.append(query.value("average").toString());
            list.append(query.value("position").toString());
        }
    }else {
        QMessageBox::critical(this, QObject::tr("Error"),
                             tr("%1").arg(query.lastError().text()));
        return QStringList();
    }

    query.exec("SELECT count(*) AS class_total FROM result_sheet");
    if(query.next()){
        list.append(QString::number((query.value("class_total").toInt() + 7 )));
    }
    else {
            QMessageBox::critical(this, QObject::tr("Error"),
                                 tr("%1").arg(query.lastError().text()));
    }

    return list;
}

void GwResult::createWindow()
{
    if(!textEdit->document()->isEmpty()){
        QPoint h = pos() + QPoint(30, 30);
        auto *newWindow = new GwResult;
        newWindow->move(h);
        newWindow->db = db;
        newWindow->db.setDatabaseName(db.databaseName());
        if(!newWindow->db.isOpen()){
            newWindow->db.open();
        }

        newWindow->termInfo = getTermInfo();

        if(!newWindow->createReportSheet()){
            delete newWindow;
            return;
        }
        newWindow->show();
        return;
    }

    createReportSheet();
}

void GwResult::writeSetting()
{
    QSettings settings(QCoreApplication::organizationName(), "Report Slip");
    settings.setValue("geometry", saveGeometry());
}

void GwResult::readSetting()
{
    QSettings settings(QCoreApplication::organizationName(), "Report Slip");
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(1000, 700);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

QPair<QString, bool> GwResult::accronym(const QString &name)
{
    if(name.size() <= 6)
        return QPair<QString, bool>(name, false);
    QStringList list = name.trimmed().split(" ", QString::KeepEmptyParts, Qt::CaseInsensitive);
    if(1 == list.size()){
        if(name.startsWith("math", Qt::CaseInsensitive)){
            return QPair<QString, bool>("Math", true);
        }
        return QPair<QString, bool>(capitalizeEachWord(name.mid(0, 3)), true);
    }
    if(2 == list.size()){
        QString str1 = list.at(0);
        QString ch = str1.at(0);
        QString str2 = list.at(1);
        QString s = str2.mid(0, 3);
        QString acc = ch.toUpper() + ". " + capitalizeEachWord(s) + "." ;
        return QPair<QString, bool>(acc, true);
    }

    if(list.contains("And"))
        list.removeAll("And");
    QString str, s;
    foreach (s, list) {
        QString ch = s.at(0);
        str += ch.toUpper() + ".";
    }
    return QPair<QString, bool>(str, true);
}

QString GwResult::principalComment(double average, QString pos)
{
    QMap<QString, QStringList> comments;
    QStringList excellent;
    excellent << "Excellent Performance. Keep it up."
              << "You are the champion! Do not let any body overthrow you."
              << "Excellent result. Do not lose focus next term."
              << "Excellent Performance. But you can take the first position. Work harder next term."
              << "Excellent but You can do better than this by beating those up before you";


    QStringList veryGood;
    veryGood << "Excellent position, but the average didn't worth it. You can do better."
             << "Near Excellent. Your can do better by working harder."
             << "Very Good result. Lettle more effort can make it excellent."
             << "You can be the champion. It is in you. Very Good perfomance.";

    QStringList good;
    good << "Very Good position, but the average didn't worth it. You can do better."
         << "Your performance is good but you can do better than this."
         << "You had a good result. Make sure you better it next term."
         << "Hard work yeild good result. Work harder to better your pefprmance."
         << "This is an incuragingly good good result. There is room for inprovement to it better";

    QStringList fair;
    fair << "Good position, but the average didn't worth it. You can do better."
         << "You realy need to improve"
         << "You need to work harder next term"
         << "You can really do well.";

    QStringList poor;
     poor << "Fair position, but the average didn't worth it. You can improve on it."
          << "This is poor performance. Please improve on your studies."
          << "Your performance is poor. Please do well to improve next term."
          << "Your result is not fair. Do more work on yourself academically to better your performance"
          << "your performance is not all that good. Make sure you improve on it next term";

     QStringList bad;
     bad << "This is a very poor result. Please change for better."
         << "Not good. Make sure you sit up next term."
         << "You need to concentrate on your studies."
         << "If you continue with thhis performance you be expel from the school";

     comments["excellent"] = excellent;
     comments["very good"] = veryGood;
     comments["good"] = good;
     comments["fair"] = fair;
     comments["poor"] = poor;
     comments["bad"] = bad;

     QStringList comment;
     if(average >= 75.0){
         if( pos == "1st" ){
             comment = comments["excellent"];
             return comment[randomInt(3)];
         }else if(pos == "2nd" || pos == "3rd"){
             comment = comments["excellent"];
             return comment[3];
         }else{
             comment = comments["excellent"];
             return comment[4];
         }
     }
     if (average >= 65.0 && average < 75.0) {
         if( pos == "1st" || pos == "2nd" || pos == "3rd" ){
             comment = comments["very good"];
             return comment[0];
         }else if(pos == "4th" || pos == "5th" || pos == "6th" || pos == "7th"){
             comment = comments["very good"];
             return comment[1];
         }else{
             comment = comments["very good"];
             return comment[randomInt(2)+2];
         }
     }
     if(average >= 55.0 && average < 65.0){
         if( pos == "1st" || pos == "2nd" || pos == "3rd" || pos == "4th" || pos == "5th" || pos == "6th" || pos == "7th" ){
             comment = comments["good"];
             return comment[0];
         }else{
             comment = comments["good"];
             return comment[randomInt(4)+1];
         }
     }
     if(average >= 40.0 && average < 55.0){
         if( pos == "1st" || pos == "2nd" || pos == "3rd" || pos == "4th"){
             comment = comments["fair"];
             return comment[0];
         }else{
             comment = comments["fair"];
             return comment[randomInt(3)+1];
         }
     }
     if(average >= 30.0 && average < 40.0){
         if(pos == "5th" || pos == "6th" || pos == "7th" ){
             comment = comments["poor"];
             return comment[0];
         }else{
             comment = comments["poor"];
             return comment[randomInt(4)+1];
         }
     }else{
         comment = comments["bad"];
         return comment[randomInt(4)];
     }
     return "";
}

int GwResult::randomInt(int i)
{
    srand(time(nullptr));
    rand();
    return rand()%i;
}

bool GwResult::saveAs()
{
    QString filePath = "C:/Users/IykeBest/ResultManager/2017_2018/2ndTerm/JSS_1/";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), filePath,
                                                    tr("Student Result File (*.srf)"));
    if(fileName.isEmpty())
        return false;

    curFileName = fileName;
    setCurrentFileName(curFileName);
    return writeFile(fileName);
}

bool GwResult::save()
{
    if(curFileName.isEmpty())
        return saveAs();


    bool success = writeFile(curFileName);
    if(!success)
        return false;
    textEdit->document()->setModified(false);
    return true;
}

bool GwResult::open()
{
    QString filePath = "C:/Users/IykeBest/ResultManager/2017_2018/2ndTerm/JSS_1/";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Save File"), filePath,
                                                    tr("Student Result File (*.srf)"));

    if(fileName.isEmpty())
        return false;

    setCurrentFileName(fileName);
    return readFile(fileName);
}

bool GwResult::okToContinue()
{
    if (!textEdit->document()->isModified())
        return true;

    const QMessageBox::StandardButton ret =
        QMessageBox::warning(this, QCoreApplication::applicationName(),
                             tr("The document has been modified.\n"
                                "Do you want to save your changes?"),
                             QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
        return save();
    else if (ret == QMessageBox::Cancel)
        return false;
    return true;
}

void GwResult::documentModified()
{
    setWindowModified(textEdit->document()->isModified());
}

void GwResult::filePrint()
{
#if QT_CONFIG(printdialog)
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (textEdit->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted)
        textEdit->print(&printer);
    delete dlg;
#endif
}

void GwResult::filePrintPreview()
{
#if QT_CONFIG(printpreviewdialog)
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, &GwResult::printPreview);
    preview.exec();
#endif
}

void GwResult::filePrintPdf()
{
#ifndef QT_NO_PRINTER
    QFileDialog fileDialog(this, tr("Export PDF"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
    fileDialog.setDefaultSuffix("pdf");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString fileName = fileDialog.selectedFiles().first();
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    textEdit->document()->print(&printer);
    statusBar()->showMessage(tr("Exported \"%1\"")
                             .arg(QDir::toNativeSeparators(fileName)));
#endif
}

void GwResult::printPreview(QPrinter *printer)
{
#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    textEdit->print(printer);
#endif
}

void GwResult::clipboardDataChanged()
{
#ifndef QT_NO_CLIPBOARD
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        pasteAction->setEnabled(md->hasText());
#endif
}

void GwResult::textBold()
{
    QTextCharFormat charFormat;
    charFormat.setFontWeight(textBoldAction->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(charFormat);
}

void GwResult::textUnderline()
{
    QTextCharFormat charFormat;
    charFormat.setFontUnderline(textUnderlineAction->isChecked());
    mergeFormatOnWordOrSelection(charFormat);
}

void GwResult::textItalic()
{
    QTextCharFormat charFormat;
    charFormat.setFontItalic(textItalicAction->isChecked());
    mergeFormatOnWordOrSelection(charFormat);
}

void GwResult::textFamily(const QString &fontFamily)
{
    QTextCharFormat charFormat;
    charFormat.setFontFamily(fontFamily);
    mergeFormatOnWordOrSelection(charFormat);
}

void GwResult::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat charFormat;
        charFormat.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(charFormat);
    }
}

void GwResult::textStyle(int styleIndex)
{
    QTextCursor cursor = textEdit->textCursor();

    if (styleIndex != 0) {
        QTextListFormat::Style style = QTextListFormat::ListDisc;

        switch (styleIndex) {
            default:
            case 1:
                style = QTextListFormat::ListDisc;
                break;
            case 2:
                style = QTextListFormat::ListCircle;
                break;
            case 3:
                style = QTextListFormat::ListSquare;
                break;
            case 4:
                style = QTextListFormat::ListDecimal;
                break;
            case 5:
                style = QTextListFormat::ListLowerAlpha;
                break;
            case 6:
                style = QTextListFormat::ListUpperAlpha;
                break;
            case 7:
                style = QTextListFormat::ListLowerRoman;
                break;
            case 8:
                style = QTextListFormat::ListUpperRoman;
                break;
        }

        cursor.beginEditBlock();

        QTextBlockFormat blockFormat = cursor.blockFormat();

        QTextListFormat listFormat;

        if (cursor.currentList()) {
            listFormat = cursor.currentList()->format();
        } else {
            listFormat.setIndent(blockFormat.indent() + 1);
            blockFormat.setIndent(0);
            cursor.setBlockFormat(blockFormat);
        }

        listFormat.setStyle(style);

        cursor.createList(listFormat);

        cursor.endEditBlock();
    } else {
        // ####
        QTextBlockFormat blockFormat1;
        blockFormat1.setObjectIndex(-1);
        cursor.mergeBlockFormat(blockFormat1);
    }
}

void GwResult::textColor()
{
    QColor color = QColorDialog::getColor(textEdit->textColor(), this);
    if (!color.isValid())
        return;
    QTextCharFormat charFormat;
    charFormat.setForeground(color);
    mergeFormatOnWordOrSelection(charFormat);
    colorChanged(color);
}

void GwResult::textAlign(QAction *alignment)
{
    if (alignment == alignLeftAction)
        textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (alignment == alignCenterAction)
        textEdit->setAlignment(Qt::AlignHCenter);
    else if (alignment == alignRightAction)
        textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (alignment == alignJustifyAction)
        textEdit->setAlignment(Qt::AlignJustify);
}

void GwResult::closeEvent(QCloseEvent *event)
{
    if (okToContinue()){
        writeSetting();
        event->accept();
    }
    else{
        event->ignore();
    }
}
