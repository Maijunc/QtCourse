#include "chatserver.h"

ChatServer::ChatServer(QObject *parent):
    QTcpServer(parent)
{}

void ChatServer::incomingConnection(qintptr socketDescriptor)
{
    ServerWorker *worker = new ServerWorker(this);
    if(!worker->setSocketDescriptor(socketDescriptor)) {
        worker->deleteLater();
        return;
    }

    m_clients.append(worker);
    emit logMessage("新的用户连接上了");
}

void ChatServer::stopServer()
{
    close();
}
