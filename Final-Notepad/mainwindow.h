#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "aboutdialog.h"
#include "searchdialog.h"
#include "replacedialog.h"
#include "codeeditor.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QColorDialog>
#include <QFontDialog>

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

private slots:
    void on_actionAbout_triggered();

    void on_actionFind_triggered();

    void on_actionReplace_triggered();

    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionSaveAs_triggered();

    void on_textEdit_textChanged();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    // void on_textEdit_copyAvailable(bool b);

    // void on_textEdit_undoAvailable(bool b);

    // void on_textEdit_redoAvailable(bool b);

    void on_actionFontColor_triggered();

    void on_actionFontBackgroundColor_triggered();

    void on_actionBackgroundColor_triggered();

    void on_actionLineWrap_triggered();

    void on_actionFont_triggered();

    void on_actionShowToolBar_triggered();

    void on_actionShowStatusBar_triggered();

    void on_actionSelectAll_triggered();

    void on_actionExit_triggered();

    void on_textEdit_cursorPositionChanged();

    void on_actionShowLineNumber_triggered(bool checked);

    void on_textTabWidget_currentChanged(int index);

private:
    Ui::MainWindow *ui;

    QLabel statusLabel;
    QLabel statusCursorLabel;

    // QMap<int, QString> m_filePaths; // 用于存储每个选项卡的文件路径

    // bool textChanged;

    bool userEditConfirmed();

    void updateEditMode(CodeEditor *editor);

    CodeEditor* getCurrentEditor();

    QWidget* getCurrentTab();

    void createTab(QString tabName);

    QString getFileNameFromPath(QString filePath);
};
#endif // MAINWINDOW_H
