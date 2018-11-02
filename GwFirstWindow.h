#ifndef GWFIRSTWINDOW_H
#define GWFIRSTWINDOW_H

#include <QMainWindow>
#include <QMap>

namespace Ui {
class GwFirstWindow;
}

class GwFirstWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GwFirstWindow(QWidget *parent = nullptr);
    ~GwFirstWindow();

    void setUpMainWindow();
    void openManager();
    bool writeFile(const QString &path, const QMap<QString, QString> classInfo);
    void readFile(const QString &fileName);

private slots:
    void on_pushButton_clicked();
    void on_actionNewConnection_triggered();

    void on_actionOpenConnection_triggered();

    void on_openManagerPushButton_clicked();

private:
    Ui::GwFirstWindow *ui;
};

#endif // GWFIRSTWINDOW_H
