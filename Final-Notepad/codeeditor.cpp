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

// calculates the width of the LineNumberArea widget
int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

SyntaxHighlighter *CodeEditor::getHighlighter()
{
    return m_syntaxHighlighter;
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



void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
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

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);

    // 动态背景颜色
    QColor backgroundColor = (this->styleSheet().contains("1E1E1E")) ? QColor("#252526") : Qt::lightGray;
    QColor textColor = (this->styleSheet().contains("1E1E1E")) ? QColor("#858585") : Qt::black;

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


