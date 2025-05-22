#include <QCoreApplication>
#include "core.h"
#include "mysql/CMySql.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QDebug>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    ICore *p = core::getKernel();
    if(p->open()){
        qDebug()<<"Server is running";
    }else{
        qDebug()<<"Server failed";
    }
    return a.exec();
}
