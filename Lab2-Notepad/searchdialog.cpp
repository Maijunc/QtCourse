#include "searchdialog.h"
#include "ui_searchdialog.h"

SearchDialog::SearchDialog(QWidget *parent, QPlainTextEdit *textEdit)
    : QDialog(parent)
    , ui(new Ui::SearchDialog)
{
    ui->setupUi(this);

    pTextEdit = textEdit;
    ui->rbDown->setChecked(true);
}

SearchDialog::~SearchDialog()
{
    delete ui;
}

void SearchDialog::on_btFindNext_clicked()
{
    QString target = ui->searchText->text();

    QString text = pTextEdit->toPlainText();
    QTextCursor c = pTextEdit->textCursor(); //获取当前光标的位置

    int index = -1;
    // 从当前光标的位置开始找

    if(ui->rbDown->isChecked()) {
        index = text.indexOf(target, c.position(), ui->cbCaseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);
        if(index >= 0){
            c.setPosition(index); //开始的位置
            c.setPosition(index + target.length(), QTextCursor::KeepAnchor); //结束的位置

            pTextEdit->setTextCursor(c);
        } else{
            // 再从文件开头找
            index = text.indexOf(target, 0, ui->cbCaseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);
            if(index >= 0) {
                c.setPosition(index); //开始的位置
                c.setPosition(index + target.length(), QTextCursor::KeepAnchor); //结束的位置

                pTextEdit->setTextCursor(c);
            }
        }
    } else if(ui->rbUp->isChecked()) {
        index = text.lastIndexOf(target, c.position() - text.length() - 1, ui->cbCaseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive) ;
        if(index >= 0){
            c.setPosition(index + target.length()); //开始的位置
            c.setPosition(index, QTextCursor::KeepAnchor); //结束的位置

            pTextEdit->setTextCursor(c);
        } else{
            // 再从文件开头找
            index = text.lastIndexOf(target, pTextEdit->toPlainText().size(), ui->cbCaseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);
            if(index >= 0) {
                c.setPosition(index + target.length()); //开始的位置
                c.setPosition(index, QTextCursor::KeepAnchor); //结束的位置

                pTextEdit->setTextCursor(c);
            }
        }
    }

    if(index < 0) {
        QMessageBox msg(this);
        msg.setWindowTitle("记事本");
        msg.setText(QString("找不到") + target);
        msg.setWindowFlag(Qt::Drawer);
        msg.setIcon(QMessageBox::Information);
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
    }
}


void SearchDialog::on_btCancel_clicked()
{
    accept();
}

