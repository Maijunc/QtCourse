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
    } else if (language == "JAVA") {
        setupJavaRules();
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
void SyntaxHighlighter::setupPythonRules() {
    // 关键字格式
    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns = {
        "\\bclass\\b", "\\bdef\\b", "\\bif\\b", "\\belse\\b", "\\belif\\b", "\\bwhile\\b",
        "\\bfor\\b", "\\btry\\b", "\\bexcept\\b", "\\bimport\\b", "\\bfrom\\b", "\\bNone\\b",
        "\\bTrue\\b", "\\bFalse\\b", "\\blambda\\b", "\\bglobal\\b", "\\bnonlocal\\b",
        "\\breturn\\b", "\\byield\\b", "\\bassert\\b", "\\bcontinue\\b", "\\bbreak\\b"
    };
    for (const QString &pattern : keywordPatterns) {
        HighlightRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightRules.append(rule);
    }

    // 类名格式（以大写字母开头的单词）
    classFormat.setForeground(Qt::darkMagenta);
    HighlightRule classRule;
    classRule.pattern = QRegularExpression("\\b[A-Z][a-zA-Z0-9_]*\\b");
    classRule.format = classFormat;
    highlightRules.append(classRule);

    // 函数格式（以小写字母或下划线开头并且后接括号）
    functionFormat.setForeground(Qt::darkGreen);
    HighlightRule functionRule;
    functionRule.pattern = QRegularExpression("\\b[a-zA-Z_][a-zA-Z0-9_]*(?=\\()");
    functionRule.format = functionFormat;
    highlightRules.append(functionRule);

    // 字符串格式（单引号和双引号的字符串）
    stringFormat.setForeground(Qt::darkRed);
    HighlightRule stringRule;
    stringRule.pattern = QRegularExpression("\"[^\"]*\"|'[^']*'");
    stringRule.format = stringFormat;
    highlightRules.append(stringRule);

    // 注释格式（单行注释）
    commentFormat.setForeground(Qt::gray);
    HighlightRule commentRule;
    commentRule.pattern = QRegularExpression("#[^\n]*");
    commentRule.format = commentFormat;
    highlightRules.append(commentRule);

    // 多行注释（Python 中的三引号注释）
    HighlightRule multiLineCommentRule;
    multiLineCommentRule.pattern = QRegularExpression("'''[^\"]*'''|\"\"\"[^\"]*\"\"\"");
    multiLineCommentRule.format = commentFormat;
    highlightRules.append(multiLineCommentRule);
}

void SyntaxHighlighter::setupJavaRules() {
    // 关键字格式
    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns = {
        "\\bclass\\b", "\\bpublic\\b", "\\bprivate\\b", "\\bprotected\\b",
        "\\bstatic\\b", "\\bfinal\\b", "\\bvoid\\b", "\\bint\\b", "\\bdouble\\b",
        "\\bif\\b", "\\belse\\b", "\\bwhile\\b", "\\bfor\\b", "\\btry\\b", "\\bcatch\\b",
        "\\breturn\\b", "\\bnew\\b", "\\bimport\\b", "\\bpackage\\b"
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
    functionRule.pattern = QRegularExpression("\\b[a-zA-Z_][a-zA-Z0-9_]*(?=\\()");
    functionRule.format = functionFormat;
    highlightRules.append(functionRule);

    // 字符串格式
    stringFormat.setForeground(Qt::darkRed);
    HighlightRule stringRule;
    stringRule.pattern = QRegularExpression("\".*\"");
    stringRule.format = stringFormat;
    highlightRules.append(stringRule);

    // 注释格式
    commentFormat.setForeground(Qt::gray);
    HighlightRule singleLineCommentRule;
    singleLineCommentRule.pattern = QRegularExpression("//[^\n]*");
    singleLineCommentRule.format = commentFormat;
    highlightRules.append(singleLineCommentRule);

    HighlightRule multiLineCommentRule;
    multiLineCommentRule.pattern = QRegularExpression("/\\*.*\\*/");
    multiLineCommentRule.format = commentFormat;
    highlightRules.append(multiLineCommentRule);
}


void SyntaxHighlighter::setupJavaScriptRules() {
    // 关键字格式
    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns = {
        "\\bvar\\b", "\\bfunction\\b", "\\breturn\\b", "\\bif\\b", "\\belse\\b",
        "\\bfor\\b", "\\bwhile\\b", "\\bconst\\b", "\\blet\\b", "\\bswitch\\b",
        "\\bcase\\b", "\\bbreak\\b", "\\bdefault\\b", "\\bnew\\b", "\\btry\\b",
        "\\bcatch\\b", "\\bthrow\\b", "\\btypeof\\b"
    };
    for (const QString &pattern : keywordPatterns) {
        HighlightRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightRules.append(rule);
    }

    // 函数格式
    functionFormat.setForeground(Qt::darkGreen);
    HighlightRule functionRule;
    functionRule.pattern = QRegularExpression("\\b[a-zA-Z_][a-zA-Z0-9_]*(?=\\()");
    functionRule.format = functionFormat;
    highlightRules.append(functionRule);

    // 字符串格式
    stringFormat.setForeground(Qt::darkRed);
    HighlightRule stringRule;
    stringRule.pattern = QRegularExpression("\".*\"|'.*'|`.*`");
    stringRule.format = stringFormat;
    highlightRules.append(stringRule);

    // 注释格式
    commentFormat.setForeground(Qt::gray);
    HighlightRule singleLineCommentRule;
    singleLineCommentRule.pattern = QRegularExpression("//[^\n]*");
    singleLineCommentRule.format = commentFormat;
    highlightRules.append(singleLineCommentRule);

    HighlightRule multiLineCommentRule;
    multiLineCommentRule.pattern = QRegularExpression("/\\*.*\\*/");
    multiLineCommentRule.format = commentFormat;
    highlightRules.append(multiLineCommentRule);
}

void SyntaxHighlighter::setupHtmlRules() {
    // 标签格式
    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);
    HighlightRule tagRule;
    tagRule.pattern = QRegularExpression("</?\\w+\\b[^>]*>");
    tagRule.format = keywordFormat;
    highlightRules.append(tagRule);

    // 属性名格式
    classFormat.setForeground(Qt::darkMagenta);
    HighlightRule attributeRule;
    attributeRule.pattern = QRegularExpression("\\b[a-zA-Z-]+(?=\\=)");
    attributeRule.format = classFormat;
    highlightRules.append(attributeRule);

    // 属性值格式
    stringFormat.setForeground(Qt::darkRed);
    HighlightRule valueRule;
    valueRule.pattern = QRegularExpression("\"[^\"]*\"|'[^']*'");
    valueRule.format = stringFormat;
    highlightRules.append(valueRule);

    // 注释格式
    commentFormat.setForeground(Qt::gray);
    HighlightRule commentRule;
    commentRule.pattern = QRegularExpression("<!--.*?-->");
    commentRule.format = commentFormat;
    highlightRules.append(commentRule);
}

