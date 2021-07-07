#include "mainwindow.h"

#include <QApplication>
#include <QTextStream>
#include <QFile>
//QString readTextFile(QString stylesheet){
//    QFile file{stylesheet};
//    if(file.open(QFile::ReadOnly | QFile::Text)){
//        QTextStream read{&file};
//        return read.readAll();
//    }
//    return "";
//}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    QString css = readTextFile(":/resources/styles.css");
//    if(css.length() >  0)
//        a.setStyleSheet(css);
    MainWindow w;
    w.show();
    return a.exec();
}
