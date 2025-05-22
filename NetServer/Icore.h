#ifndef ICORE_H
#define ICORE_H
#include <QTcpSocket>
#include <fstream>
#include <string>
#include <ctime>
#include <QObject>
class ICore : public QObject{
    Q_OBJECT
public:
    ICore(){};
    virtual ~ICore(){};
public:
    virtual bool open() = 0;
    virtual void close() = 0;
    Q_INVOKABLE virtual void dealData(QTcpSocket* sock,char*szbuf) = 0;
};

#endif // ICORE_H
