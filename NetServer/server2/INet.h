#ifndef INET_H
#define INET_H
#include "tou.h"
#include <QObject>
class INet:public QObject{
    Q_OBJECT
public:
    explicit INet(QObject *parent = nullptr){}
    virtual ~INet(){}
public:
    virtual bool initNetWork(const QString& szip = HOSTIP,quint16 sport = HOSTPORT)=0;
    virtual void unInitNetWork()=0;
    virtual bool sendData(QTcpSocket* sock,const char* data,int nlen)=0;
    virtual QMap<quint64, QTcpSocket*>& getClientSockets()=0;
signals:
    void dataReceived(QTcpSocket* socket, QByteArray data);  // 数据接收信号
};

#endif // INET_H
