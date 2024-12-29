#include "codeeditor.h"

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);
    m_currentLineColor = QColor("#FFFACD");

    m_syntaxHighlighter = new SyntaxHighlighter(document());
    // m_syntaxHighlighter->setLanguage(QString("C++")); // 默认设置为 C++ 高亮

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    setMouseTracking(true);
}
void CodeEditor::showBookmarks() {
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("书签");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    QListWidget *listWidget = new QListWidget(dialog);

    // 填充书签列表
    for (int lineNumber : m_Bookmarks) {
        QListWidgetItem *item = new QListWidgetItem(
            QString("行 %1").arg(lineNumber + 1), // 行号从 0 开始，需要加 1
            listWidget
            );
        item->setData(Qt::UserRole, lineNumber); // 将行号存储为用户数据
        listWidget->addItem(item);
    }

    QPushButton *closeButton = new QPushButton("关闭", dialog);
    layout->addWidget(listWidget);
    layout->addWidget(closeButton);

    dialog->setLayout(layout);

    // 双击跳转到书签所在行
    connect(listWidget, &QListWidget::itemDoubleClicked, [this, dialog](QListWidgetItem *item) {
        int lineNumber = item->data(Qt::UserRole).toInt();
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, lineNumber);
        setTextCursor(cursor);
        dialog->accept(); // 关闭窗口
    });

    // 添加右键菜单
    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(listWidget, &QListWidget::customContextMenuRequested, [this, listWidget](const QPoint &pos) {
        QMenu menu;
        QAction *deleteAction = menu.addAction("删除书签");

        QAction *selectedAction = menu.exec(listWidget->mapToGlobal(pos));
        if (selectedAction == deleteAction) {
            QListWidgetItem *item = listWidget->itemAt(pos);
            if (item) {
                int lineNumber = item->data(Qt::UserRole).toInt();
                m_Bookmarks.remove(lineNumber); // 从 QSet 中移除书签
                delete item; // 从列表中移除
                update(); // 刷新界面
            }
        }
    });

    // 关闭按钮
    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);

    dialog->exec();
}


QSet<int> CodeEditor::getBookMarks()
{
    return this->m_Bookmarks;
}

void CodeEditor::toggleBookmark(int lineNumber) {
    if (m_Bookmarks.contains(lineNumber)) {
        m_Bookmarks.remove(lineNumber); // 移除书签
    } else {
        m_Bookmarks.insert(lineNumber); // 添加书签
    }
    lineNumberArea->update(); // 更新行号区域显示
}

void LineNumberArea::mouseMoveEvent(QMouseEvent *e) {

    if (e->pos().x() < codeEditor->lineNumberAreaWidth()) {
        setCursor(Qt::PointingHandCursor);
        QToolTip::showText(e->globalPosition().toPoint(), "Click to toggle bookmark");
    } else {
        setCursor(Qt::IBeamCursor);
        QToolTip::hideText();
    }

    QWidget::mouseMoveEvent(e);
}

void LineNumberArea::mousePressEvent(QMouseEvent* e)
{
    // 检测点击是否发生在行号区域
    if (e->button() == Qt::LeftButton) {
        QRect lineNumberAreaRect(0, 0, codeEditor->lineNumberAreaWidth(), height());
        if (lineNumberAreaRect.contains(e->pos())) {
            QTextCursor cursor = codeEditor->cursorForPosition(QPoint(codeEditor->lineNumberAreaWidth(), e->pos().y()));
            int lineNumber = cursor.blockNumber();
            codeEditor->toggleBookmark(lineNumber); // 切换书签
        }
    } else {
        // 如果未点击行号区域，执行默认处理
        QWidget::mousePressEvent(e);
    }
}


void CodeEditor::mousePressEvent(QMouseEvent* e)
{
    QPlainTextEdit::mousePressEvent(e);

    if (e->button() != Qt::LeftButton)
        return;

    // 使用 QRegularExpression 来匹配 URL
    QRegularExpression urlRex("(http|ftp|https):\\/\\/[\\w\\-_]+(\\.[\\w\\-_]+)+([\\w\\-\\.,@?^=%&:/~\\+#]*[\\w\\-\\@?^=%&/~\\+#])?");

    // 获取鼠标点击的位置
    QTextCursor cursor = textCursor();
    QString str = cursor.block().text();
    int blockPos = cursor.positionInBlock();

    // 查找匹配的链接
    int pos = 0;
    while (pos < str.length()) {
        QRegularExpressionMatch match = urlRex.match(str.mid(pos));

        if (match.hasMatch()) {
            int matchStart = match.capturedStart();
            int matchLength = match.capturedLength();

            // 如果点击位置在链接内，打开链接
            if (blockPos - matchStart < matchLength) {
                QString urlStr = str.mid(matchStart, matchLength);
                QUrl url(urlStr);
                QDesktopServices::openUrl(url);
                break;
            }

            pos += matchLength;
        } else {
            break;
        }
    }
}

void CodeEditor::applyTheme(const QString &theme) {
    if (theme == "dark") {
        this->setStyleSheet("background-color: #1E1E1E; color: #D4D4D4;");
        lineNumberArea->setStyleSheet("background-color: #252526; color: #858585;");
        m_currentLineColor = QColor("#264F78"); // 深色主题高亮行的背景颜色
        // m_currentLineColor = QColor("#FFFACD"); // 浅色主题高亮行的背景颜色

    } else {
        this->setStyleSheet("background-color: #FFFFFF; color: #000000;");
        lineNumberArea->setStyleSheet("background-color: #F3F3F3; color: #555555;");
        m_currentLineColor = QColor("#FFFACD"); // 浅色主题高亮行的背景颜色
        // m_currentLineColor = QColor("#264F78"); // 深色主题高亮行的背景颜色

    }

    // 通知高亮器更改颜色
    if (m_syntaxHighlighter) {
        m_syntaxHighlighter->applyTheme(theme);
    }

    highlightCurrentLine(); // 重新高亮当前行
}

SyntaxHighlighter *CodeEditor::getHighlighter()
{
    return m_syntaxHighlighter;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

// calculates the width of the LineNumberArea widget
int CodeEditor::lineNumberAreaWidth() {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    // 行号宽度 + 书签标记额外空间
    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    // 额外预留空间（书签标记）
    int bookmarkSpace = fontMetrics().height(); // 使用字体高度作为书签标记的宽度
    space += bookmarkSpace;

    return space;
}


void CodeEditor::resizeEvent(QResizeEvent *e) {
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}



void CodeEditor::updateLineNumberArea(const QRect &rect, int dy) {
    if (dy) {
        lineNumberArea->scroll(0, dy);
    } else {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0); // 确保宽度更新
    }
}


void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        // 动态设置当前行高亮颜色
        selection.format.setBackground(m_currentLineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);

        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}
void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event) {
    QPainter painter(lineNumberArea);

    // 动态背景颜色
    QColor backgroundColor = (this->styleSheet().contains("1E1E1E")) ? QColor("#252526") : Qt::lightGray;
    QColor textColor = (this->styleSheet().contains("1E1E1E")) ? QColor("#858585") : Qt::black;
    QColor bookmarkColor = QColor("#FFAA00"); // 书签颜色

    painter.fillRect(event->rect(), backgroundColor); // 背景颜色

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(textColor); // 根据主题动态调整行号颜色
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);

            // 绘制书签标记
            if (m_Bookmarks.contains(blockNumber)) {
                int markerSize = fontMetrics().height() / 2;
                QRect markerRect(2, top + (fontMetrics().height() - markerSize) / 2, markerSize, markerSize);
                painter.setBrush(bookmarkColor);
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(markerRect);
            }
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::showLineNumberArea(bool flag)
{
    if(flag)
    {
        lineNumberArea->show();
        setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
    }
    else
    {
        lineNumberArea->hide();
        setViewportMargins(0, 0, 0, 0);
    }
}


