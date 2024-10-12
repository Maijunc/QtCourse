#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStack>
#include <math.h>
#include <QDebug>
#include <QKeyEvent>
#include <QMap>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString operand;
    QString opcode;
    bool markEqual;
    QStack<QString> operands;
    QStack<QString> opcodes;
    QMap<int, QPushButton*> digitBtns;

    QString calculation(bool *ok = nullptr);

private slots:
    void btnNumClicked();

    void on_btnPeriod_clicked();

    void on_btnDelete_clicked();

    void on_btnClear_clicked();

    void btnBinaryOperatorClicked();

    void btnUnaryOperatorClicked();

    void on_btnEqual_clicked();

    virtual void keyPressEvent(QKeyEvent *event);

    void on_btnClearRecent_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
