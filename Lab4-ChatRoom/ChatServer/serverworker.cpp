#include "serverworker.h"

ServerWorker::ServerWorker(QObject *parent)
    : QObject{parent}
{
    m_serverSocket = new QTcpSocket(this);
    connect(m_serverSocket, &QTcpSocket::readyRead, this, &ServerWorker::onReadyRead);
}

bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor)
{
    return m_serverSocket->setSocketDescriptor(socketDescriptor);
}

void ServerWorker::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Version::Qt_6_7);
    // start an infinite loop
    while(true) {
        socketStream.startTransaction();
        socketStream >> jsonData;
        // 收到一条消息就把内容传给mainWindow
        if(socketStream.commitTransaction()) {
            emit logMessage(QString::fromUtf8(jsonData));
            // 把消息发送回去
            sendMessage("I recieved message");
        } else {
            break;
        }
    }
}

void ServerWorker::sendMessage(const QString &text, const QString &type)
{
    if(m_serverSocket->state() != QAbstractSocket::ConnectedState)
        return;

    if(!text.isEmpty()) {
        // create a QDataStream operating on the socket
        QDataStream serverStream(m_serverSocket);
        serverStream.setVersion(QDataStream::Version::Qt_6_7);

        // create the JSON we want to send;
        QJsonObject message;
        message["type"] = type;
        message["text"] = text;

        // send the JSON using QDataStream
        serverStream << QJsonDocument(message).toJson();
    }
}
