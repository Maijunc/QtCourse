#include "syntaxhighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {}

void SyntaxHighlighter::setLanguage(const QString &language) {
    highlightRules.clear();
    if (language == "C++") {
        setupCppRules();
    } else if (language == "Python") {
        setupPythonRules();
    } else if (language == "JavaScript") {
        setupJavaScriptRules();
    } else if (language == "HTML") {
        setupHtmlRules();
    }
    rehighlight(); // 强制重新高亮
}

void SyntaxHighlighter::applyTheme(const QString &theme) {
    if (theme == "dark") {
        keywordFormat.setForeground(QColor("#569CD6"));  // 关键字
        commentFormat.setForeground(QColor("#6A9955"));  // 注释
        stringFormat.setForeground(QColor("#D69D85"));   // 字符串
        // numberFormat.setForeground(QColor("#B5CEA8"));   // 数字
    } else {
        keywordFormat.setForeground(Qt::blue);
        commentFormat.setForeground(Qt::darkGreen);
        stringFormat.setForeground(Qt::darkRed);
        // numberFormat.setForeground(Qt::darkMagenta);
    }

    rehighlight(); // 强制重新高亮
}


void SyntaxHighlighter::highlightBlock(const QString &text) {
    for (const HighlightRule &rule : highlightRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}

void SyntaxHighlighter::setupCppRules() {
    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns = {
        "\\bclass\\b", "\\bconst\\b", "\\bint\\b", "\\bvoid\\b", "\\bif\\b", "\\belse\\b", "\\bwhile\\b"
    };
    for (const QString &pattern : keywordPatterns) {
        HighlightRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightRules.append(rule);
    }

    // 类名格式
    classFormat.setForeground(Qt::darkMagenta);
    HighlightRule classRule;
    classRule.pattern = QRegularExpression("\\b[A-Z][a-zA-Z0-9_]*\\b");
    classRule.format = classFormat;
    highlightRules.append(classRule);

    // 函数格式
    functionFormat.setForeground(Qt::darkGreen);
    HighlightRule functionRule;
    functionRule.pattern = QRegularExpression("\\b[a-zA-Z0-9_]+(?=\\()");
    functionRule.format = functionFormat;
    highlightRules.append(functionRule);

    // 字符串
    stringFormat.setForeground(Qt::darkRed);
    HighlightRule stringRule;
    stringRule.pattern = QRegularExpression("\".*\"");
    stringRule.format = stringFormat;
    highlightRules.append(stringRule);

    // 单行注释
    commentFormat.setForeground(Qt::gray);
    HighlightRule commentRule;
    commentRule.pattern = QRegularExpression("//[^\n]*");
    commentRule.format = commentFormat;
    highlightRules.append(commentRule);
}

void SyntaxHighlighter::setupPythonRules()
{
    // Python keywords
    QStringList keywords = {"def", "class", "if", "elif", "else", "try", "except", "import", "from", "return"};
    foreach (const QString &keyword, keywords) {
        HighlightRule rule;
        rule.pattern = QRegularExpression("\\b" + keyword + "\\b");
        rule.format = keywordFormat;
        highlightRules.append(rule);
    }

    // Python comments
    HighlightRule commentRule;
    commentRule.pattern = QRegularExpression("#[^\n]*");
    commentRule.format = commentFormat;
    highlightRules.append(commentRule);

    // Python string literals
    HighlightRule stringRule;
    stringRule.pattern = QRegularExpression("\".*\"|'.*'");
    stringRule.format = stringFormat;
    highlightRules.append(stringRule);
}

void SyntaxHighlighter::setupJavaScriptRules()
{
    // JavaScript keywords
    QStringList keywords = {"function", "var", "let", "const", "if", "else", "return", "for", "while"};
    foreach (const QString &keyword, keywords) {
        HighlightRule rule;
        rule.pattern = QRegularExpression("\\b" + keyword + "\\b");
        rule.format = keywordFormat;
        highlightRules.append(rule);
    }

    // JavaScript comments
    HighlightRule commentRule;
    commentRule.pattern = QRegularExpression("//[^\n]*|/\\*.*\\*/");
    commentRule.format = commentFormat;
    highlightRules.append(commentRule);

    // JavaScript string literals
    HighlightRule stringRule;
    stringRule.pattern = QRegularExpression("\".*\"|'.*'");
    stringRule.format = stringFormat;
    highlightRules.append(stringRule);
}

void SyntaxHighlighter::setupHtmlRules()
{
    // HTML tags
    HighlightRule tagRule;
    tagRule.pattern = QRegularExpression("<[^>]+>");
    tagRule.format = keywordFormat;
    highlightRules.append(tagRule);

    // HTML attributes
    HighlightRule attributeRule;
    attributeRule.pattern = QRegularExpression("\\b[a-zA-Z-]+(?=\\=)");
    attributeRule.format = classFormat;
    highlightRules.append(attributeRule);

    // HTML string literals
    HighlightRule stringRule;
    stringRule.pattern = QRegularExpression("\".*\"|'.*'");
    stringRule.format = stringFormat;
    highlightRules.append(stringRule);
}
