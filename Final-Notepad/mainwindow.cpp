#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // this->textChanged = false;
    on_actionNew_triggered();

    this->statusLabel.setMaximumWidth(150);
    this->statusLabel.setText("length: " + QString::number(0) + "    lines: " + QString::number(1));
    ui->statusbar->addPermanentWidget(&statusLabel);

    this->statusCursorLabel.setMaximumWidth(150);
    this->statusCursorLabel.setText("Row: " + QString::number(1) + "    Col: " + QString::number(1));
    ui->statusbar->addPermanentWidget(&statusCursorLabel);

    QLabel *author = new QLabel(ui->statusbar);
    author->setText("麦天骏");
    ui->statusbar->addPermanentWidget(author);

    ui->actionCopy->setEnabled(false);
    ui->actionCut->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);
    ui->actionPaste->setEnabled(false);

    ui->actionShowToolBar->setChecked(true);
    ui->actionShowStatusBar->setChecked(true);

    CodeEditor *editor = getCurrentEditor();

    updateEditMode(editor);


    favoritesManager.loadFavorites();

    // 创建菜单
    QMenuBar *menuBar = this->menuBar();
    favoritesMenu = menuBar->addMenu("收藏夹");

    QAction *addFavoriteAction = new QAction("添加到收藏夹", this);
    connect(addFavoriteAction, &QAction::triggered, this, &MainWindow::addFavorite);
    favoritesMenu->addAction(addFavoriteAction);

    favoritesMenu->addSeparator();
    updateFavoritesMenu();

    // QAction *manageFavoritesAction = new QAction("管理收藏夹", this);
    // connect(manageFavoritesAction, &QAction::triggered, this, &MainWindow::manageFavorites);
    // favoritesMenu->addAction(manageFavoritesAction);

    recentFilesMenu = menuBar->addMenu("最近打开");
    loadRecentFiles();
    updateRecentFilesMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::addFavorite() {
    QString filePath = QFileDialog::getOpenFileName(this, "选择文件");
    if (!filePath.isEmpty()) {
        favoritesManager.addFavorite(filePath);
        favoritesManager.saveFavorites();
        updateFavoritesMenu();
    }
}

void MainWindow::openFavorite() {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        QString filePath = action->data().toString();
        if (!QFile::exists(filePath)) {
            QMessageBox::warning(this, "错误", "文件不存在：" + filePath);
        } else {
            openFile(filePath);
        }
        // 读取文件并展示内容...
        addRecentFile(filePath); // 添加到历史记录
    }
}

void MainWindow::updateFavoritesMenu() {
    // 清除旧的动态动作
    QList<QAction *> oldActions = favoritesMenu->actions();
    for (QAction *action : oldActions) {
        if (action->text() != "添加到收藏夹") {
            favoritesMenu->removeAction(action);
            delete action;
        }
    }

    // 添加新的收藏项
    for (const QString &path : favoritesManager.getFavorites()) {
        // 创建收藏项的菜单
        QMenu *subMenu = new QMenu(QFileInfo(path).fileName(), favoritesMenu);

        // 添加跳转按钮
        QAction *openAction = new QAction("打开", subMenu);
        openAction->setData(path);
        connect(openAction, &QAction::triggered, this, &MainWindow::openFavorite);

        // 添加删除按钮
        QAction *deleteAction = new QAction("删除", subMenu);
        connect(deleteAction, &QAction::triggered, this, [=]() {
            favoritesManager.removeFavorite(path);
            favoritesManager.saveFavorites();
            updateFavoritesMenu();
        });

        // 将按钮添加到子菜单
        subMenu->addAction(openAction);
        subMenu->addAction(deleteAction);

        // 添加子菜单到收藏夹菜单
        favoritesMenu->addMenu(subMenu);
    }
}



void MainWindow::manageFavorites() {
    QList<QString> favorites = favoritesManager.getFavorites();
    QString pathToDelete = QInputDialog::getItem(this, "删除收藏", "选择要删除的路径：", favorites, 0, false);

    if (!pathToDelete.isEmpty()) {
        favoritesManager.removeFavorite(pathToDelete);
        favoritesManager.saveFavorites();
        updateFavoritesMenu();
    }
}


void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dlg;
    dlg.exec();
}


void MainWindow::on_actionFind_triggered()
{
    CodeEditor *editor = getCurrentEditor();

    SearchDialog dlg(this, editor);
    dlg.exec();
}


void MainWindow::on_actionReplace_triggered()
{
    CodeEditor *editor = getCurrentEditor();

    ReplaceDialog dlg(this, editor);
    dlg.exec();
}


void MainWindow::on_actionNew_triggered()
{
    createTab(QString("untitled"));

    // textChanged = false;
    getCurrentEditor()->setProperty("textChanged", false);
}


void MainWindow::on_actionOpen_triggered()
{
    // if(!userEditConfirmed())
    //     return;

    QString filePath = QFileDialog::getOpenFileName(this, "保存文件", ".", tr("Text Files (*.txt);;"
                                                                              "C/C++ Files (*.c *.cpp *.h);;"
                                                                              "Python Files (*.py);;"
                                                                              "JavaScript Files (*.js);;"
                                                                              "HTML Files (*.html *.htm);;"
                                                                              "Java Files (*.java);;"
                                                                              "All Files (*.*)"));

    QFile file(filePath);

    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "..", "打开文件失败");
        return;
    }
    QString fileName = getFileNameFromPath(filePath);
    createTab(fileName);

    // 获取当前文本编辑
    CodeEditor *editor = getCurrentEditor();

    QTextStream in(&file);
    QString text = in.readAll();
    editor->insertPlainText(text);
    file.close();

    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());

    editor->setProperty("textChanged", false);
    editor->setProperty("filePath", filePath);

    // 设置语法高亮语言
    QString extension = QFileInfo(fileName).suffix().toLower();
    if (extension == "cpp" || extension == "c" || extension == "h") {
        getCurrentEditor()->getHighlighter()->setLanguage("C++");
    } else if (extension == "py") {
        getCurrentEditor()->getHighlighter()->setLanguage("Python");
    } else if (extension == "js") {
        getCurrentEditor()->getHighlighter()->setLanguage("JavaScript");
    } else if (extension == "html" || extension == "htm") {
        getCurrentEditor()->getHighlighter()->setLanguage("HTML");
    } else if (extension == "java") {
        getCurrentEditor()->getHighlighter()->setLanguage("Java");
    } else {
        getCurrentEditor()->getHighlighter()->setLanguage("PlainText");
    }

    // 读取文件并展示内容...
    addRecentFile(filePath); // 添加到历史记录

}


void MainWindow::on_actionSave_triggered()
{
    CodeEditor* editor = getCurrentEditor();

    QString filePath = editor->property("filePath").toString();

    QFile file(filePath);

    if(filePath == "")
    {
        QString filePath = QFileDialog::getSaveFileName(this, "保存文件", ".", tr("Text Files (*.txt);;"
                                                                                  "C/C++ Files (*.c *.cpp *.h);;"
                                                                                  "Python Files (*.py);;"
                                                                                  "JavaScript Files (*.js);;"
                                                                                  "HTML Files (*.html *.htm);;"
                                                                                  "Java Files (*.java);;"
                                                                                  "All Files (*.*)"));


        file.setFileName(filePath);
        if(!file.open(QFile::WriteOnly | QFile::Text))
        {
            QMessageBox::warning(this, ".." , "保存文件失败");
            return;
        }
        // m_filePath = fileName;
        editor->setProperty("filePath", filePath);

    }
    else if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, ".." , "保存文件失败");
        return;
    }

    this->setWindowTitle(editor->property("filePath").toString());
    QTabWidget *tabWidget = ui->textTabWidget;
    tabWidget->setTabText(tabWidget->currentIndex(), getFileNameFromPath(editor->property("filePath").toString()));

    QTextStream out(&file);

    QString text = editor->toPlainText();
    out << text;
    file.flush();
    file.close();

    editor->setProperty("textChanged", false);
}


void MainWindow::on_actionSaveAs_triggered()
{
    CodeEditor* editor = getCurrentEditor();

    QString filePath = QFileDialog::getSaveFileName(this, "保存文件", ".", tr("Text Files (*.txt);;"
                                                                              "C/C++ Files (*.c *.cpp *.h);;"
                                                                              "Python Files (*.py);;"
                                                                              "JavaScript Files (*.js);;"
                                                                              "HTML Files (*.html *.htm);;"
                                                                              "Java Files (*.java);;"
                                                                              "All Files (*.*)"));

    QFile file(filePath);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, ".." , "保存文件失败");
        return;
    }

    getCurrentEditor()->setProperty("filePath", filePath);
    // m_filePath = fileName;

    QTextStream out(&file);
    QString text = getCurrentEditor()->toPlainText();
    out << text;
    file.flush();
    file.close();

    QString fileName = getFileNameFromPath(filePath);

    this->setWindowTitle(editor->property("filePath").toString());
    QTabWidget *tabWidget = ui->textTabWidget;
    tabWidget->setTabText(tabWidget->currentIndex(), getFileNameFromPath(editor->property("filePath").toString()));

    this->setWindowTitle(QFileInfo(getCurrentEditor()->property("filePath").toString()).absoluteFilePath());

    QString extension = QFileInfo(fileName).suffix().toLower();
    if (extension == "cpp" || extension == "c" || extension == "h") {
        getCurrentEditor()->getHighlighter()->setLanguage("C++");
    } else if (extension == "py") {
        getCurrentEditor()->getHighlighter()->setLanguage("Python");
    } else if (extension == "js") {
        getCurrentEditor()->getHighlighter()->setLanguage("JavaScript");
    } else if (extension == "html" || extension == "htm") {
        getCurrentEditor()->getHighlighter()->setLanguage("HTML");
    } else if (extension == "java") {
        getCurrentEditor()->getHighlighter()->setLanguage("Java");
    } else {
        getCurrentEditor()->getHighlighter()->setLanguage("PlainText");
    }


}


void MainWindow::on_textEdit_textChanged()
{
    bool textChanged = getCurrentEditor()->property("textChanged").toBool();

    if (!textChanged) {
        this->setWindowTitle("*" + this->windowTitle());
        getCurrentEditor()->setProperty("textChanged", true);
        // textChanged = true;
    }

    // 获取信号的发送者
    CodeEditor *editor = qobject_cast<CodeEditor *>(sender());
    if (!editor) return;

    // 更新状态栏中的信息
    this->statusLabel.setText(
        "length: " + QString::number(editor->toPlainText().length()) +
        "    lines: " + QString::number(editor->document()->lineCount())
        );

}

// 检查是否保存
bool MainWindow::userEditConfirmed()
{
    CodeEditor* editor = getCurrentEditor();
    bool textChanged = editor->property("textChanged").toBool();
    // qDebug() << "userEditConfirmed " << "textChanged = " << textChanged;
    QString filePath = editor->property("filePath").toString();

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
                editor->setProperty("textChanged", false);
                break;
            case QMessageBox::Cancel:
                return false;
        }
    }
    return true;
}

void MainWindow::updateEditMode(CodeEditor *editor)
{
    QPlainTextEdit::LineWrapMode mode = editor->lineWrapMode();

    if(mode == QPlainTextEdit::NoWrap) {
        ui->actionLineWrap->setChecked(false);
    } else {
        ui->actionLineWrap->setChecked(true);
    }

    ui->actionShowLineNumber->setChecked(true);

}

CodeEditor *MainWindow::getCurrentEditor()
{
    QTabWidget* tabWidget = ui->textTabWidget;
    // QWidget *currentTab = tabWidget->currentWidget();
    // CodeEditor *editor = currentTab->findChild<CodeEditor *>();
    CodeEditor *editor = qobject_cast<CodeEditor *>(tabWidget->currentWidget());
    // if (editor) {
    //     qDebug() << "当前选项卡的 CodeEditor" << editor;
    // }
    return editor;
}

void MainWindow::createTab(QString tabName)
{
    // 获取 TabWidget
    QTabWidget* tabWidget = ui->textTabWidget;

    // 创建一个新的 QTextEdit 编辑器
    CodeEditor *editor = new CodeEditor;

    // 添加新选项卡
    int newIndex = tabWidget->addTab(editor, tabName);

    // 设置新选项卡为当前选项卡
    tabWidget->setCurrentIndex(newIndex);

    // 设置焦点到新编辑器
    editor->setFocus();

    this->setWindowTitle(tabName);

    // 设置当前选项卡的文件路径，默认为空
    editor->setProperty("filePath", "");
    // 设置默认为没修改
    editor->setProperty("textChanged", false);

    // 连接光标位置变化信号到槽函数
    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::on_textEdit_cursorPositionChanged);

    connect(editor, &QPlainTextEdit::textChanged, this, &MainWindow::on_textEdit_textChanged);

    connect(editor, &QPlainTextEdit::copyAvailable, this, &MainWindow::on_textEdit_copyAvailable);

    connect(editor, &QPlainTextEdit::undoAvailable, this, &MainWindow::on_textEdit_undoAvailable);

    connect(editor, &QPlainTextEdit::redoAvailable, this, &MainWindow::on_textEdit_redoAvailable);
}

QString MainWindow::getFileNameFromPath(QString filePath)
{
    // 创建 QFileInfo 对象
    QFileInfo fileInfo(filePath);

    // 获取文件名（包括扩展名）
    QString fileName = fileInfo.fileName();

    return fileName;
}

void MainWindow::toggleTheme(bool darkMode) {
    QString theme = darkMode ? "dark" : "light";

    // 遍历所有 CodeEditor 实例，应用新主题
    foreach (CodeEditor *editor, findChildren<CodeEditor *>()) {
        editor->applyTheme(theme);
    }

    // 设置全局样式
    if (darkMode) {
        qApp->setStyleSheet("QMainWindow { background-color: #2E2E2E; color: #FFFFFF; }");
    } else {
        qApp->setStyleSheet("QMainWindow { background-color: #FFFFFF; color: #000000; }");
    }
}


void MainWindow::on_actionUndo_triggered()
{
    // ui->textEdit->undo();
    getCurrentEditor()->undo();
}


void MainWindow::on_actionRedo_triggered()
{
    // ui->textEdit->redo();
    getCurrentEditor()->redo();
}


void MainWindow::on_actionCut_triggered()
{
    // ui->textEdit->cut();
    getCurrentEditor()->cut();
    ui->actionPaste->setEnabled(true);
}


void MainWindow::on_actionCopy_triggered()
{
    // ui->textEdit->copy();
    getCurrentEditor()->copy();
    ui->actionPaste->setEnabled(true);
}


void MainWindow::on_actionPaste_triggered()
{
    getCurrentEditor()->paste();
    // ui->textEdit->paste();
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


void MainWindow::on_actionFontColor_triggered()
{
    CodeEditor *editor = getCurrentEditor();

    QColor color = QColorDialog::getColor(Qt::black, this, "选择颜色");
    if(color.isValid()){
        editor->setStyleSheet(editor->styleSheet() + QString("QPlainTextEdit {color: %1}").arg(color.name()));
    }
}


void MainWindow::on_actionFontBackgroundColor_triggered()
{
    CodeEditor *editor = getCurrentEditor();

    QColor color = QColorDialog::getColor(Qt::black, this, "选择颜色");
    if(color.isValid()){
        editor->setStyleSheet(editor->styleSheet() + QString("QPlainTextEdit {background-color: %1}").arg(color.name()));
    }
}


void MainWindow::on_actionBackgroundColor_triggered()
{

}


void MainWindow::on_actionLineWrap_triggered()
{
    CodeEditor *editor = getCurrentEditor();

    QPlainTextEdit::LineWrapMode mode = editor->lineWrapMode();

    if(mode == QPlainTextEdit::NoWrap) {
        editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);

        ui->actionLineWrap->setChecked(true);
    } else {
        editor->setLineWrapMode(QPlainTextEdit::NoWrap);

        ui->actionLineWrap->setChecked(false);
    }
}


void MainWindow::on_actionFont_triggered()
{
    CodeEditor *editor = getCurrentEditor();

    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, this);

    if(ok)
        editor->setFont(font);
}


void MainWindow::on_actionShowToolBar_triggered()
{
    bool visible = ui->toolBar->isVisible();
    ui->toolBar->setVisible(!visible);
    ui->actionShowToolBar->setChecked(!visible);

}


void MainWindow::on_actionShowStatusBar_triggered()
{
    bool visible = ui->statusbar->isVisible();
    ui->statusbar->setVisible(!visible);
    ui->actionShowStatusBar->setChecked(!visible);
}


void MainWindow::on_actionSelectAll_triggered()
{
    CodeEditor *editor = getCurrentEditor();

    editor->selectAll();
}


void MainWindow::on_actionExit_triggered()
{
    if(userEditConfirmed())
        exit(0);
}

void MainWindow::on_textEdit_cursorPositionChanged() {
    // 获取信号的发送者
    CodeEditor *editor = qobject_cast<CodeEditor *>(sender());
    if (!editor) return; // 如果发送者不是 CodeEditor，则返回

    // 初始化列和行
    int col = 0;
    int row = 0;
    int flg = -1;

    // 获取光标位置和内容
    int pos = editor->textCursor().position();
    QString text = editor->toPlainText();

    // 计算行列号
    for (int i = 0; i < pos; i++) {
        if (text[i] == '\n') {
            row++;
            flg = i;
        }
    }
    flg++;
    col = pos - flg;

    // 更新状态栏
    this->statusCursorLabel.setText("Row: " + QString::number(row + 1) + "    Col: " + QString::number(col + 1));
}

void MainWindow::on_actionShowLineNumber_triggered(bool checked)
{
    CodeEditor *editor = getCurrentEditor();

    editor->showLineNumberArea(checked);
}


void MainWindow::on_textTabWidget_currentChanged(int index)
{
    QTabWidget *tabWidget = ui->textTabWidget;

    this->setWindowTitle(tabWidget->widget(index)->property("filePath").toString() == "" ? "untitle" : tabWidget->widget(index)->property("filePath").toString());
}


void MainWindow::on_actionToggleTheme_triggered()
{
    static bool darkMode = false; // 默认浅色模式
    darkMode = !darkMode;
    toggleTheme(darkMode);
}


void MainWindow::on_actionCheckBookmarks_triggered()
{
    CodeEditor *editor = getCurrentEditor();

    editor->showBookmarks();
}

void MainWindow::loadRecentFiles() {
    QSettings settings("YourCompany", "YourApp");
    recentFiles = settings.value("recentFiles").toStringList();
}

void MainWindow::saveRecentFiles() {
    QSettings settings("YourCompany", "YourApp");
    settings.setValue("recentFiles", recentFiles);
}

void MainWindow::addRecentFile(const QString &filePath) {
    recentFiles.removeAll(filePath); // 移除重复路径
    recentFiles.prepend(filePath);  // 添加到列表开头

    if (recentFiles.size() > maxRecentFiles) {
        recentFiles = recentFiles.mid(0, maxRecentFiles); // 限制大小
    }

    updateRecentFilesMenu(); // 更新菜单
    saveRecentFiles();       // 保存到设置
}

void MainWindow::updateRecentFilesMenu() {
    recentFilesMenu->clear();

    for (const QString &filePath : recentFiles) {
        QAction *action = new QAction(filePath, this);
        connect(action, &QAction::triggered, [this, filePath]() {
            // 打开文件逻辑
            openFile(filePath);
        });
        recentFilesMenu->addAction(action);
    }

    if (!recentFiles.isEmpty()) {
        recentFilesMenu->addSeparator();
        QAction *clearAction = new QAction("清除历史记录", this);
        connect(clearAction, &QAction::triggered, this, &MainWindow::clearRecentFiles);
        recentFilesMenu->addAction(clearAction);
    }
}

void MainWindow::clearRecentFiles() {
    recentFiles.clear();
    updateRecentFilesMenu();
    saveRecentFiles();
}


void MainWindow::openFile(QString filePath)
{
    // 打开文件逻辑
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件：" + filePath);
        return;
    }
    QTextStream in(&file);
    QString text = in.readAll();
    file.close();

    QString fileName = QFileInfo(filePath).fileName();
    createTab(fileName);
    CodeEditor *editor = getCurrentEditor();
    editor->setPlainText(text);

    // 设置文件属性
    editor->setProperty("filePath", filePath);
    editor->setProperty("textChanged", false);

    // 更新窗口标题
    this->setWindowTitle(filePath);

    // 设置语法高亮语言
    QString extension = QFileInfo(fileName).suffix().toLower();
    if (extension == "cpp" || extension == "c" || extension == "h") {
        editor->getHighlighter()->setLanguage("C++");
    } else if (extension == "py") {
        editor->getHighlighter()->setLanguage("Python");
    } else if (extension == "js") {
        editor->getHighlighter()->setLanguage("JavaScript");
    } else if (extension == "html" || extension == "htm") {
        editor->getHighlighter()->setLanguage("HTML");
    } else if (extension == "java") {
        editor->getHighlighter()->setLanguage("Java");
    } else {
        editor->getHighlighter()->setLanguage("PlainText");
    }
}
