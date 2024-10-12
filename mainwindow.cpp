#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    digitBtns = {{Qt::Key_0, ui->btnNum0},
               {Qt::Key_1, ui->btnNum1},
               {Qt::Key_2, ui->btnNum2},
               {Qt::Key_3, ui->btnNum3},
               {Qt::Key_4, ui->btnNum4},
               {Qt::Key_5, ui->btnNum5},
               {Qt::Key_6, ui->btnNum6},
               {Qt::Key_7, ui->btnNum7},
               {Qt::Key_8, ui->btnNum8},
               {Qt::Key_9, ui->btnNum9},
               };

    foreach(auto btn, digitBtns)
        connect(btn, SIGNAL(clicked()), this, SLOT(btnNumClicked()));

    connect(ui->btnPlus, SIGNAL(clicked()), this, SLOT(btnBinaryOperatorClicked()));
    connect(ui->btnMinus, SIGNAL(clicked()), this, SLOT(btnBinaryOperatorClicked()));
    connect(ui->btnMulti, SIGNAL(clicked()), this, SLOT(btnBinaryOperatorClicked()));
    connect(ui->btnDivide, SIGNAL(clicked()), this, SLOT(btnBinaryOperatorClicked()));

    connect(ui->btnPercentage, SIGNAL(clicked()), this, SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnInverse, SIGNAL(clicked()), this, SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnSquare, SIGNAL(clicked()), this, SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnSqrt, SIGNAL(clicked()), this, SLOT(btnUnaryOperatorClicked()));
    connect(ui->btnPlusMinus, SIGNAL(clicked()), this, SLOT(btnUnaryOperatorClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::calculation(bool* ok)
{
    double result = 0;
    if(operands.size() == 2 && opcodes.size() > 0)
    {
        // 取操作数
        double operand1 = operands.front().toDouble();
        operands.pop_front();
        double operand2 = operands.front().toDouble();
        operands.pop_front();

        // 取操作符
        QString op = opcodes.front();
        opcodes.pop_front();

        if(op == "+")
            result = operand1 + operand2;
        else if(op == "-")
            result = operand1 - operand2;
        else if(op == "×")
            result = operand1 * operand2;
        else if(op == "÷")
            result = operand1 / operand2;

        operands.push_back(QString::number(result));

        ui->statusbar->showMessage(QString("calculation is in progress: operands is %1, opcodes is %2").arg(operands.size()).arg(opcodes.size()));

        if(ok)
            *ok = true;
    }else
    {
        ui->statusbar->showMessage(QString("operands is %1, opcodes is %2").arg(operands.size()).arg(opcodes.size()));
    }

    return QString::number(result);
}

void MainWindow::btnNumClicked()
{
    QString digit = qobject_cast<QPushButton*>(sender())->text();

    if(digit == "0" && operand == "0")
        digit = "";

    if(operand == "0" && digit != "0")
        operand = 0;

    operand += digit;
    ui->display->setText(operand);

}

void MainWindow::on_btnPeriod_clicked()
{
    if(!operand.contains(".") && !operand.isEmpty())
        operand += qobject_cast<QPushButton*>(sender())->text();

    ui->display->setText(operand);
}


void MainWindow::on_btnDelete_clicked()
{
    if(markEqual)
    {
        operand = operands.front();
        operands.pop_front();
        markEqual = false;
    }
    operand = operand.left(operand.length() - 1);

    ui->display->setText(operand);
}


void MainWindow::on_btnClear_clicked()
{
    operand.clear();
    operands.clear();
    opcodes.clear();

    ui->display->setText(operand);
}

void MainWindow::btnBinaryOperatorClicked()
{
    QString opcode = qobject_cast<QPushButton*>(sender())->text();

    if(operand != "") //完成equal操作，此时operand为空，但是operands不为空，理应可以继续添加操作符 但是正常来说上次按完下次不能连着按
    {
        operands.push_back(operand);
        operand = "";

        opcodes.push_back(opcode);

        bool ok = false;
        QString result = calculation(&ok);

        if(ok)
            ui->display->setText(operands.front());
    }else if(markEqual)
    {
        opcodes.push_back(opcode);
        markEqual = false;
    }
}

void MainWindow::btnUnaryOperatorClicked()
{
    if(markEqual)
    {
        operand = operands.front();
        operands.pop_front();
        markEqual = false;
    }
    if(operand != "")
    {
        double result = operand.toDouble();
        operand = "";

        QString op = qobject_cast<QPushButton*>(sender())->text();

        if(op == "%")
            result /= 100.0;
        else if(op == "1/x")
            result = 1 / result;
        else if(op == "x^2")
            result *= result;
        else if(op == "√")
            result = sqrt(result);
        else if(op == "±")
            result = -result;

        operand = QString::number(result);

        ui->display->setText(operand);
    }
}

void MainWindow::on_btnEqual_clicked()
{
    if(operand != "")
    {
        operands.push_back(operand);
        operand = "";
        markEqual = true;

        bool ok = false;
        QString result = calculation(&ok);
        if(ok) //表示此时是通过等号完成的运算
            ui->display->setText(operands.front());
        ui->statusbar->showMessage(QString("operand is %1, operands.front() is %2").arg(operand).arg(operands.front()));
    }

}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    foreach(auto btnKey, digitBtns.keys())
    {
        if(event->key() == btnKey)
            digitBtns[btnKey]->animateClick();
    }

}


void MainWindow::on_btnClearRecent_clicked()
{
    operand.clear();
    ui->display->setText(operand);
}

