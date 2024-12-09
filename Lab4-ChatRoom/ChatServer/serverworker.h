#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>

class ServerWorker : public QObject
{
    Q_OBJECT
public:
    explicit ServerWorker(QObject *parent = nullptr);
    virtual bool setSocketDescriptor(qintptr socketDescriptor);

signals:
    void logMessage(const QString &msg);

private:
    QTcpSocket * m_serverSocket;

public slots:
    void onReadyRead();
    void sendMessage(const QString &text, const QString &type = "message");
};

#endif // SERVERWORKER_H
