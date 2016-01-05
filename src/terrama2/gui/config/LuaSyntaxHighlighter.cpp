/*
  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.

  This file is part of TerraMA2 - a free and open source computational
  platform for analysis, monitoring, and alert of geo-environmental extremes.

  TerraMA2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.

  TerraMA2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with TerraMA2. See LICENSE. If not, write to
  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
*/

/*!
  \file terrama2/gui/config/LuaSyntaxHighlighter.cpp

  \brief Definition of Class LuaSyntaxHighlighter

  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

// TerraMA2
#include "LuaSyntaxHighlighter.hpp"

// QT
#include <QRegExp>
#include <QTextCharFormat>
#include <QTextEdit>

terrama2::gui::config::LuaSyntaxHighlighter::LuaSyntaxHighlighter(QTextEdit* widget)
 : QSyntaxHighlighter(widget->document())
{
  HighlightingRule rule;

  QTextCharFormat keywordFormat;
  keywordFormat.setForeground(Qt::darkBlue);
  keywordFormat.setFontWeight(QFont::Bold);
  QStringList keywordPatterns;
  keywordPatterns << "\\band\\b"    << "\\bbreak\\b"  << "\\bdo\\b"
                  << "\\belse\\b"   << "\\belseif\\b" << "\\bend\\b"
                  << "\\bfalse\\b"  << "\\bfor\\b"    << "\\bfunction\\b"
                  << "\\bif\\b"     << "\\bin\\b"     << "\\blocal\\b"
                  << "\\bnil\\b"    << "\\bnot\\b"    << "\\bor\\b"
                  << "\\brepeat\\b" << "\\breturn\\b" << "\\bthen\\b"
                  << "\\btrue\\b"   << "\\buntil\\b"  << "\\bwhile\\b"
                  << "\\{"          << "\\}";

  foreach(QString pattern, keywordPatterns)
  {
    rule.pattern = QRegExp(pattern);
    rule.format  = keywordFormat;
    rules.append(rule);
  } 

  QTextCharFormat quotationFormat;
  quotationFormat.setForeground(Qt::darkGreen);

  rule.pattern = QRegExp("\"[^\"]*\"");
  rule.format = quotationFormat;
  rules.append(rule);

  rule.pattern = QRegExp("'[^']*'");
  rule.format = quotationFormat;
  rules.append(rule);

  QTextCharFormat commentFormat;
  commentFormat.setForeground(Qt::red);
  rule.pattern = QRegExp("--[^\n]*");
  rule.format  = commentFormat;
  rules.append(rule);
  
  // Parameters to fill up
  QTextCharFormat gapFormat;
  gapFormat.setBackground(Qt::yellow);
  rule.pattern = QRegExp("\\b_\\w+_\\b");
  rule.format  = gapFormat;

  rules.append(rule);
  rule.pattern = QRegExp("\\b_\\.\\.\\._\\b");
  rule.format  = gapFormat;

  rules.append(rule); 
}

void terrama2::gui::config::LuaSyntaxHighlighter::highlightBlock(const QString& text)
{
  for(HighlightingRule rule: rules)
  {
    QRegExp expression(rule.pattern);
    int index = text.indexOf(expression);
    while(index >= 0)
    {
      int length = expression.matchedLength();
      setFormat(index, length, rule.format);
      index = text.indexOf(expression, index + length);
    }
  } 
}
