#include "tcpnet.h"
#include "core.h"
#include <QThreadPool>

class ClientTask : public QRunnable {
public:
    explicit ClientTask(QTcpSocket* socket) : clientSocket(socket) {}

    void run() override {
        if (!clientSocket || !clientSocket->isOpen()) return;

        QByteArray data = clientSocket->readAll();
        if (data.isEmpty()) return;

        //qDebug() << "Thread" << QThread::currentThreadId() << "processing data:" << data;

        QMetaObject::invokeMethod(core::getKernel(), "dealData",
                                  Qt::QueuedConnection,
                                  Q_ARG(QTcpSocket*, clientSocket),
                                  Q_ARG(char*, data.data()));
    }

private:
    QTcpSocket* clientSocket;
};

TCPNet::TCPNet(QObject* parent) : INet(parent), server(new QTcpServer(this)) {
    connect(server, &QTcpServer::newConnection, this, &TCPNet::onNewConnection);
    QThreadPool::globalInstance()->setMaxThreadCount(8);  // 设置最大并发线程数
}

bool TCPNet::initNetWork(const QString& ip, quint16 port) {
    if (!server->listen(QHostAddress(ip), port)) {
        qDebug() << "Server Start Failed: " << server->errorString();
        return false;
    }
    qDebug() << "Server Listening on:" << ip << ":" << port;
    return true;
}

QMap<quint64, QTcpSocket *> &TCPNet::getClientSockets() {
    return clientSockets;
}
void TCPNet::unInitNetWork() {
    // 停止服务器并清理所有连接
    if (server->isListening()) {
        server->close();
    }
    for (auto socket : clientSockets) {
        socket->disconnectFromHost();
        socket->deleteLater();
    }
    clientSockets.clear();
}

bool TCPNet::sendData(QTcpSocket* socket, const char* data, int len) {
    if (!socket || socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Socket is not connected!";
        return false;
    }

    int totalBytesSent = 0;
    while (totalBytesSent < len) {
        qint64 bytesSent = socket->write(data + totalBytesSent, len - totalBytesSent);

        if (bytesSent == -1) {
            qDebug() << "Error sending data:" << socket->errorString();
            return false;  // 发生错误，停止发送
        }

        totalBytesSent += bytesSent;

        if (!socket->waitForBytesWritten(3000)) {  // 等待数据写入完成
            qDebug() << "Timeout while sending data!";
            return false;
        }
    }

    socket->flush();  // 确保数据尽快发送
    //qDebug() << "Data sent successfully, total bytes:" << totalBytesSent;
    return true;
}
void TCPNet::onNewConnection() {
    QTcpSocket* clientSocket = server->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, &TCPNet::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &TCPNet::onClientDisconnected);

    quint64 clientId = reinterpret_cast<quint64>(clientSocket);
    clientSockets[clientId] = clientSocket;
    qDebug() << "New Client Connected:" << clientSocket->peerAddress().toString();
}

void TCPNet::onReadyRead() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    // 使用线程池处理数据
    ClientTask* task = new ClientTask(socket);
    task->setAutoDelete(true);  // 任务执行完自动释放
    QThreadPool::globalInstance()->start(task);
}

void TCPNet::onClientDisconnected() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    quint64 clientId = reinterpret_cast<quint64>(socket);
    // 先查找是否存在
    auto it = clientSockets.find(clientId);
    if (it != clientSockets.end()) {
        clientSockets.erase(it);
    }
    // 先断开信号，避免 `deleteLater` 后访问已删除对
    socket->disconnect();
    socket->deleteLater();

    qDebug() << "ClientID:"<<clientId<<" Disconnected";
}
