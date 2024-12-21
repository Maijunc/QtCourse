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
    connect(m_chatClient, &ChatClient::messageReceived, this, &MainWindow::messageReceived);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loginButton_clicked()
{
    // 连接服务器
    m_chatClient->connectToServer(QHostAddress(ui->serverEdit->text()), 1967);


}
void MainWindow::on_sayButton_clicked()
{
    // 客户端发消息，如果发的消息存在的话
    if(!ui->sayLineEdit->text().isEmpty())
        m_chatClient->sendMessage(ui->sayLineEdit->text());
}


void MainWindow::on_logoutButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
}

// 槽函数，绑定的是clientSocket的connect信号，表示socket成功连接上后主页面要做的反应
void MainWindow::connectedToServer()
{
    ui->stackedWidget->setCurrentWidget(ui->chatPage);
    m_chatClient->sendMessage(ui->usernameEdit->text(), "login");

}

void MainWindow::messageReceived(const QString &text)
{
    ui->roomTextEdit->append(text);
}



