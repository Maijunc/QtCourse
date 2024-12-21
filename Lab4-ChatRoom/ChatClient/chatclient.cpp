#include "chatclient.h"

ChatClient::ChatClient(QObject *parent): QObject{parent}
{
    m_clientSocket = new QTcpSocket(this);

    connect(m_clientSocket, &QTcpSocket::connected, this, &ChatClient::connected);
    connect(m_clientSocket, &QTcpSocket::readyRead, this, &ChatClient::onReadyRead);
}

void ChatClient::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_clientSocket);
    socketStream.setVersion(QDataStream::Version::Qt_6_7);
    // start an infinite loop
    while(true) {
        socketStream.startTransaction();
        socketStream >> jsonData;
        // 收到一条消息就把内容传给mainWindow
        if(socketStream.commitTransaction()) {
            qDebug() << "messageReceived";
            emit messageReceived(QString::fromUtf8(jsonData));
        } else {
            break;
        }
    }
}

void ChatClient::sendMessage(const QString &text, const QString &type)
{
    if(m_clientSocket->state() != QAbstractSocket::ConnectedState)
        return;

    if(!text.isEmpty()) {
        // create a QDataStream operating on the socket
        QDataStream clientStream(m_clientSocket);
        clientStream.setVersion(QDataStream::Version::Qt_6_7);

        // create the JSON we want to send;
        QJsonObject message;
        message["type"] = type;
        message["text"] = text;

        // send the JSON using QDataStream
        clientStream << QJsonDocument(message).toJson();
    }
}

void ChatClient::connectToServer(const QHostAddress &address, quint16 port)
{
    m_clientSocket->connectToHost(address, port);

}
