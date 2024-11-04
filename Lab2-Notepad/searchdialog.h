#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QDebug>
#include <QMessageBox>

namespace Ui {
class SearchDialog;
}

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = nullptr, QPlainTextEdit *textEdit = nullptr);
    ~SearchDialog();

private slots:
    void on_btFindNext_clicked();

    void on_btCancel_clicked();

private:
    Ui::SearchDialog *ui;

    QPlainTextEdit *pTextEdit;
};

#endif // SEARCHDIALOG_H
