#include "GwFirstWindow.h"
#include <QApplication>
#include <QCommandLineParser>

#include "GwGradeBookWidget.h"
#include "GwStudentView.h"
#include "GwMainWindow.h"

int main(int argc, char *argv[])
{
    //Q_INIT_RESOURCE(sdi);
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("ResultManager");
    QCoreApplication::setOrganizationName("Grand World Technology");
    QCoreApplication::setApplicationVersion("1.0");
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "The file(s) to open.");
    parser.process(a);

    GwMainWindow *mainWindow = Q_NULLPTR;

    foreach (const QString &file, parser.positionalArguments()) {
        mainWindow = new GwMainWindow(file);
        mainWindow->openResultManager();
        mainWindow->show();
    }

    if(!mainWindow){
        GwFirstWindow *firstWindow = new GwFirstWindow;
        firstWindow->show();
    }

    return a.exec();
}

