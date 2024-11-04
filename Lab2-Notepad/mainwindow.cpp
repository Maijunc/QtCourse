#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->textChanged = false;
    on_actionNew_triggered();

    this->statusLabel.setMaximumWidth(150);
    this->statusLabel.setText("length: " + QString::number(0) + "    lines: " + QString::number(1));
    ui->statusbar->addPermanentWidget(&statusLabel);

    this->statusCursorLabel.setMaximumWidth(150);
    this->statusCursorLabel.setText("Row: " + QString::number(0) + "    Col: " + QString::number(1));
    ui->statusbar->addPermanentWidget(&statusCursorLabel);

    QLabel *author = new QLabel(ui->statusbar);
    author->setText("麦天骏");
    ui->statusbar->addPermanentWidget(author);

    ui->actionCopy->setEnabled(false);
    ui->actionCut->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);
    ui->actionPaste->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dlg;
    dlg.exec();
}


void MainWindow::on_actionFind_triggered()
{
    SearchDialog dlg;
    dlg.exec();
}


void MainWindow::on_actionReplace_triggered()
{
    ReplaceDialog dlg;
    dlg.exec();
}


void MainWindow::on_actionNew_triggered()
{
    if(!userEditConfirmed())
        return;

    ui->textEdit->clear();
    this->setWindowTitle(tr("新建文本文件 - 编辑器"));

    textChanged = false;
}


void MainWindow::on_actionOpen_triggered()
{
    if(!userEditConfirmed())
        return;



    QString fileName = QFileDialog::getOpenFileName(this, "打开文件", ".", tr("Text files (*.txt) ;; All (*.*)"));
    QFile file(fileName);

    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "..", "打开文件失败");
        return;
    }
    ui->textEdit->clear();

    this->filePath = fileName;

    QTextStream in(&file);
    QString text = in.readAll();
    ui->textEdit->insertPlainText(text);
    file.close();

    this->setWindowTitle(QFileInfo(fileName).absoluteFilePath());

    textChanged = false;
}


void MainWindow::on_actionSave_triggered()
{
    QFile file(filePath);

    if(filePath == "")
    {
        QString fileName = QFileDialog::getSaveFileName(this, "保存文件", ".", tr("Text files (*.txt) "));

        file.setFileName(fileName);
        if(!file.open(QFile::WriteOnly | QFile::Text))
        {
            QMessageBox::warning(this, ".." , "保存文件失败");
            return;
        }
        filePath = fileName;
    }

    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, ".." , "保存文件失败");
        return;
    }

    QTextStream out(&file);
    QString text = ui->textEdit->toPlainText();
    out << text;
    file.flush();
    file.close();

    textChanged = false;
}


void MainWindow::on_actionSaveAs_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "保存文件", ".", tr("Text files (*.txt) "));

    QFile file(fileName);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, ".." , "保存文件失败");
        return;
    }
    filePath = fileName;

    QTextStream out(&file);
    QString text = ui->textEdit->toPlainText();
    out << text;
    file.flush();
    file.close();

    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());
}


void MainWindow::on_textEdit_textChanged()
{
    if(!textChanged)
    {
        this->setWindowTitle("*" + this->windowTitle());
        textChanged = true;
    }
}

bool MainWindow::userEditConfirmed()
{
    if(textChanged) {
        QString path = (filePath != "") ? filePath : "无标题";

        QMessageBox msg(this);
        msg.setIcon(QMessageBox::Question);
        msg.setWindowTitle("...");
        msg.setWindowFlag(Qt::Drawer);
        msg.setText(QString("是否将更改保存到\n") + "\"" + path + "\" ?");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        int r = msg.exec();
        switch(r) {
            case QMessageBox::Yes:
                on_actionSave_triggered();
                break;
            case QMessageBox::No:
                textChanged = false;
                break;
            case QMessageBox::Cancel:
                return false;
        }
    }
    return true;
}


void MainWindow::on_actionUndo_triggered()
{
    ui->textEdit->undo();
}


void MainWindow::on_actionRedo_triggered()
{
    ui->textEdit->redo();
}


void MainWindow::on_actionCut_triggered()
{
    ui->textEdit->cut();
    ui->actionPaste->setEnabled(true);
}


void MainWindow::on_actionCopy_triggered()
{
    ui->textEdit->copy();
    ui->actionPaste->setEnabled(true);
}


void MainWindow::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}


void MainWindow::on_textEdit_copyAvailable(bool b)
{
    ui->actionCopy->setEnabled(b);
    ui->actionCut->setEnabled(b);
}


void MainWindow::on_textEdit_undoAvailable(bool b)
{
    ui->actionUndo->setEnabled(b);
}


void MainWindow::on_textEdit_redoAvailable(bool b)
{
    ui->actionRedo->setEnabled(b);
}

