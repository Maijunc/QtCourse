#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QMessageBox>

namespace Ui {
class ReplaceDialog;
}

class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceDialog(QWidget *parent = nullptr, QPlainTextEdit *textEdit = nullptr);
    ~ReplaceDialog();

private slots:
    void on_btFindNext_clicked();

    void on_btCancel_clicked();

    void on_btReplace_clicked();

    void on_btReplaceAll_clicked();

private:
    Ui::ReplaceDialog *ui;
    QPlainTextEdit *pTextEdit;
};

#endif // REPLACEDIALOG_H
