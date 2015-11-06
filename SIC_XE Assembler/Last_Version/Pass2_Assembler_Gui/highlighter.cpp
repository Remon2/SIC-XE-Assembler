/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::blue);
    keywordFormat.setFontWeight(QFont::Bold);//CSECT
    QStringList keywordPatterns;
    keywordPatterns << "\\b[Cc][Ss][Ee][Cc][Tt]\\b"<< "\\b[Ss][Tt][Aa][Rr][Tt]\\b" << "\\b[eE][nN][dD]\\b" << "\\b[bB][yY][tT][eE]\\b"
                    << "\\b[wW][oO][rR][dD]\\b"  << "\\b[rR][eE][sS][bB]\\b"<< "\\b[rR][eE][sS][wW]\\b"
                    << "\\b[eE][qQ][uU]\\b"   << "\\b[oO][rR][gG]\\b"  << "\\b[lL][tT][oO][rR][gG]\\b"
                    << "\\b[bB][aA][sS][eE]\\b"   <<"\\b[nN][oO][bB][aA][sS][eE]\\b"<< "\\b[aA][dD][dD]\\b"
                    <<"\\b[aA][dD][dD][fF]\\b"   <<"\\b[aA][dD][dD][rR]\\b" <<"\\b[aA][nN][dD]\\b"
                    <<"\\b[cC][lL][eE][aA][rR]\\b"  <<"\\b[cC][oO][mM][pP]\\b" <<"\\b[cC][oO][mM][pP][fF]\\b"
                    <<"\\b[cC][oO][mM][pP][rR]\\b"  <<"\\b[dD][iI][vV]\\b"  <<"\\b[dD][iI][vV][fF]\\b"
                    <<"\\b[dD][iI][vV][rR]\\b"   <<"\\b[fF][iI][xX]\\b"  <<"\\b[fF][lL][oO][[aA][aA]][tT]\\b"
                    <<"\\b[hH][iI][oO]\\b"    <<"\\b[jJ]\\b"    <<"\\b[jJ][eE][qQ]\\b"
                    <<"\\b[jJ][gG][tT]\\b"    <<"\\b[jJ][lL][tT]\\b"  <<"\\b[jJ][sS][uU][bB]\\b"
                    <<"\\b[lL][dD][aA]\\b"    <<"\\b[lL][dD][bB]\\b"  <<"\\b[lL][dD][cC][hH]\\b"
                    <<"\\b[lL][dD][fF]\\b"    <<"\\b[lL][dD][lL]\\b"  <<"\\b[lL][dD][sS]\\b"
                    <<"\\b[lL][dD][tT]\\b"    <<"\\b[lL][dD][xX]\\b"  <<"\\b[lL][pP][sS]\\b"
                    <<"\\b[mM][uU][lL]\\b"    <<"\\b[wW][dD]\\b"   <<"\\b[mM][uU][lL][fF]\\b"
                    <<"\\b[mM][uU][lL][rR]\\b"   <<"\\b[nN][oO][rR][mM]\\b" <<"\\b[oO][rR]\\b"
                    <<"\\b[rR][dD]\\b"     <<"\\b[rR][mM][oO]\\b"  <<"\\b[rR][sS][uU][bB]\\b"
                    <<"\\b[sS][hH][iI][fF][tT][lL]\\b" <<"\\b[sS][hH][iI][fF][tT][rR]\\b"<<"\\b[sS][iI][oO]\\b"
                    <<"\\b[sS][sS][kK]\\b"    <<"\\b[sS][tT][[aA]\\b"   <<"\\b[sS][tT][bB]\\b"
                    <<"\\b[sS][tT][cC][hH]\\b"   <<"\\b[sS][tT][fF]\\b"   <<"\\b[sS][tT][iI]\\b"
                    <<"\\b[sS][tT][lL]\\b"    <<"\\b[sS][tT][sS]\\b"   <<"\\b[sS][tT][sS][wW]\\b"
                    <<"\\b[sS][tT][tT]\\b"    <<"\\b[sS][tT][xX]\\b"   <<"\\b[sS][uU][bB]\\b"
                    <<"\\b[sS][uU][bB][fF]\\b"   <<"\\b[sS][uU][bB][rR]\\b"  <<"\\b[sS][vV][cC]\\b"<<"\\b[tT][dD]\\b"
                    <<"\\b[tT][iI][oO]\\b"    <<"\\b[tT][iI][xX]\\b"<<"\\b[tT][iI][xX][rR]\\b";

    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}
