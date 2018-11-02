#include "GwConnectionDialog.h"
#include "ui_GwConnectionDialog.h"

#include "QMessageBox"
#include <QMap>
#include <QRegularExpressionValidator>
#include <QDebug>

GwConnectionDialog::GwConnectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GwConnectionDialog)
{
    ui->setupUi(this);

    //Update the class stream combo box dynamically
    QStringList list("Select Class Stream");
    for(int i = 0; i < 26; ++i){
        QChar c = 'A' + i;
        list.append(c);
    }
    ui->classStreamComboBox->addItems(list);

    auto *validate = new QRegularExpressionValidator(regex(), this);
    ui->sessionLineEdit->setValidator(validate);
}

GwConnectionDialog::~GwConnectionDialog()
{
    delete ui;
}

QString GwConnectionDialog::className(){
    if(ui->classComboBox->currentIndex() == 0){
        QMessageBox::critical(this, tr("Error Occured"),
                              tr("No class selected\n"
                                 "Please select the right class"));
        return QString();
    }
    QString cn = ui->classComboBox->currentText();
    cn.replace(' ', '_');
    return cn;
}

QString GwConnectionDialog::classStream()
{
    if(ui->classStreamComboBox->currentIndex() == 0){
        QMessageBox::critical(this, tr("Error Occured"),
                              tr("No class selected\n"
                                 "Please select the right class"));
        return QString();
    }
    QString cs = ui->classStreamComboBox->currentText();
    return cs;
}

QString GwConnectionDialog::fullClassName()
{
    if(className().isEmpty() || classStream().isEmpty())
        return QString();
    return className() + classStream();
}

QString GwConnectionDialog::term(){
    if(ui->termComboBox->currentIndex() == 0){
        QMessageBox::critical(this, tr("Error Occured"),
                              tr("No term selected\n"
                                 "Please select the right term"));
        return QString();
    }
    QString t = ui->termComboBox->currentText();
    return t;
}

QString GwConnectionDialog::session(){
    QString sess = ui->sessionLineEdit->text();
    QRegularExpressionMatch macth = regex().match(sess);
    if(!macth.hasMatch()){
        QMessageBox::critical(this, tr("Error Occured"),
                              tr("You forgot to state the academic session\n"
                                 "Please type the academic session"));
        return QString();
    }
    return sess.replace('/', '_');
}

QRegularExpression GwConnectionDialog::regex(){
    return QRegularExpression(QString("20\\d{2}/20\\d{2}"));
}

QMap<QString, QString> GwConnectionDialog::connectionData(){
    if(session().isEmpty() || term().isEmpty() || fullClassName().isEmpty()){
        return QMap<QString, QString>();
    }
    QMap<QString, QString> mapedStringPath;
    mapedStringPath["year"] = session();
    mapedStringPath["term"] = term();
    mapedStringPath["class"] = fullClassName();

    return mapedStringPath;
}

QString GwConnectionDialog::pathString()
{
    QString path= QString("%1/%2Term/%3").arg(session()).arg(term()).arg(fullClassName());
    return path;
}
