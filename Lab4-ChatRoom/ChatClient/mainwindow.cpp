#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
    m_chatClient = new ChatClient(this);

    connect(m_chatClient, &ChatClient::connected, this, &MainWindow::connectedToServer);
    // connect(m_chatClient, &ChatClient::messageReceived, this, &MainWindow::messageReceived);
    connect(m_chatClient, &ChatClient::jsonReceived, this, &MainWindow::jsonReceived);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loginButton_clicked()
{
    // 连接服务器
    m_chatClient->connectToServer(QHostAddress(ui->serverEdit->text()), 1967);
    ui->stackedWidget->setCurrentWidget(ui->loginPage);

    for( auto aItem : ui->userListWidget->findItems(ui->usernameEdit->text(), Qt::MatchExactly)) {
        qDebug("remove");
        ui->userListWidget->removeItemWidget(aItem);
        delete aItem;
    }
}
void MainWindow::on_sayButton_clicked()
{
    // 客户端发消息，如果发的消息存在的话
    if(!ui->sayLineEdit->text().isEmpty())
        m_chatClient->sendMessage(ui->sayLineEdit->text());
}


void MainWindow::on_logoutButton_clicked()
{
    m_chatClient->disconnectFromHost();
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
}

// 槽函数，绑定的是clientSocket的connect信号，表示socket成功连接上后主页面要做的反应
void MainWindow::connectedToServer()
{
    ui->stackedWidget->setCurrentWidget(ui->chatPage);
    m_chatClient->sendMessage(ui->usernameEdit->text(), "login");

}

void MainWindow::messageReceived(const QString &sender, const QString &text)
{
    ui->roomTextEdit->append(QString("%1 : %2").arg(sender).arg(text));
}

void MainWindow::jsonReceived(const QJsonObject &docObj)
{
    const QJsonValue typeVal = docObj.value("type");
    if (typeVal.isNull() || !typeVal.isString())
        return;
    if (typeVal.toString().compare("message", Qt::CaseInsensitive) == 0) {
        const QJsonValue textVal = docObj.value("text");
        const QJsonValue senderVal = docObj.value("sender");
        if (textVal.isNull() || !textVal.isString())
            return;

        if(senderVal.isNull() || !senderVal.isString())
            return;

        messageReceived(senderVal.toString(), textVal.toString());


    } else if(typeVal.toString().compare("newuser", Qt::CaseInsensitive) == 0) {
        const QJsonValue usernameVal = docObj.value("username");
        if (usernameVal.isNull() || !usernameVal.isString())
            return;

        userJoined(usernameVal.toString());
    } else if(typeVal.toString().compare("userDisconnected", Qt::CaseInsensitive) == 0) {
        const QJsonValue usernameVal = docObj.value("username");
        if (usernameVal.isNull() || !usernameVal.isString())
            return;

        userLeft(usernameVal.toString());
    } else if(typeVal.toString().compare("userlist", Qt::CaseInsensitive) == 0) {
        const QJsonValue userlistVal = docObj.value("userlist");
        if (userlistVal.isNull() || !userlistVal.isArray())
            return;
        qDebug() << userlistVal.toVariant().toStringList();
        userListReceived(userlistVal.toVariant().toStringList());
    }
}

void MainWindow::userJoined(const QString &user)
{
    qDebug() << user + "joined";
    ui->userListWidget->addItem(user);
}

void MainWindow::userLeft(const QString &user)
{
    for( auto aItem : ui->userListWidget->findItems(user, Qt::MatchExactly)) {
        qDebug("remove");
        ui->userListWidget->removeItemWidget(aItem);
        delete aItem;
    }
}

void MainWindow::userListReceived(const QStringList &list)
{
    ui->userListWidget->clear();
    ui->userListWidget->addItems(list);
}



