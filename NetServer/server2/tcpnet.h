#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QObject>
#include <QThreadPool>
#include "INet.h"

class TCPNet : public INet {
    Q_OBJECT
public:
    explicit TCPNet(QObject* parent = nullptr);
    bool initNetWork(const QString& ip, quint16 port);
    void unInitNetWork();
    bool sendData(QTcpSocket* socket, const char* data, int nlen);
    QMap<quint64, QTcpSocket*>& getClientSockets();

signals:
    void dataReceived(QTcpSocket* socket, QByteArray data);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();

private:
    QTcpServer* server;
    QMap<quint64, QTcpSocket*> clientSockets;  // 用于存储客户端连接
};

#endif // TCPSERVER_H
