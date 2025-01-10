#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include "syntaxhighlighter.h"

#include <QObject>
#include <QPlainTextEdit>
#include <QPainter>
#include <QTextBlock>
#include <QDesktopServices>
#include <QToolTip>
#include <QDialog>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QMenu>

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    SyntaxHighlighter *getHighlighter();
    void applyTheme(const QString &theme);

    void toggleBookmark(int lineNumber);

    QSet<int> getBookMarks();
    void showBookmarks();
protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *e) override;

    void mousePressEvent(QMouseEvent *e) override;
private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

public slots:
    void showLineNumberArea(bool flag);

private:
    QWidget *lineNumberArea;
    SyntaxHighlighter *m_syntaxHighlighter;

    QColor m_currentLineColor; // 当前行高亮颜色
    QSet<int> m_Bookmarks; // 存储有书签的行号
    void highlightLinks();
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor)
    {
        setMouseTracking(true); // 启用鼠标追踪
    }

    QSize sizeHint() const override
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
private:
    CodeEditor *codeEditor;


};

#endif // CODEEDITOR_H
