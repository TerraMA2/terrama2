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

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

/*!
\class LuaSyntaxHighlighter

Utiliza as regras do Qt para efetuar o highlight de um script em Lua
Efetua a marcação de:

- Comandos (if, for, while, ...)
- Strings
- Comentários
- Parâmetros a serem preenchidos (na forma _xxxx_)
*/


#include "LuaSyntaxHighlighter.hpp"


//! Construtor
LuaSyntaxHighlighter::LuaSyntaxHighlighter(QTextEdit* parent)
 : QSyntaxHighlighter(parent)
{
  HighlightingRule rule;

  // Comandos 
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
    rule._pattern = QRegExp(pattern);
    rule._format  = keywordFormat;
    _rules.append(rule);
  } 

  // Strings
  QTextCharFormat quotationFormat;
  quotationFormat.setForeground(Qt::darkGreen);
  rule._pattern = QRegExp("\"[^\"]*\"");
  rule._format = quotationFormat;
  _rules.append(rule);
  rule._pattern = QRegExp("'[^']*'");
  rule._format = quotationFormat;
  _rules.append(rule);
  
  // Comentários
  QTextCharFormat commentFormat;
  commentFormat.setForeground(Qt::red);
  rule._pattern = QRegExp("--[^\n]*");
  rule._format  = commentFormat;
  _rules.append(rule);
  
  // Parâmetros a serem preenchidos
  QTextCharFormat gapFormat;
  gapFormat.setBackground(Qt::yellow);
  rule._pattern = QRegExp("\\b_\\w+_\\b");
  rule._format  = gapFormat;
  _rules.append(rule);
  rule._pattern = QRegExp("\\b_\\.\\.\\._\\b");
  rule._format  = gapFormat;
  _rules.append(rule); 
}

//! Implementa regras de formatação configuradas
void LuaSyntaxHighlighter::highlightBlock(const QString &text)
{
  foreach(HighlightingRule rule, _rules)
  {
    QRegExp expression(rule._pattern);
    int index = text.indexOf(expression);
    while(index >= 0)
    {
      int length = expression.matchedLength();
      setFormat(index, length, rule._format);
      index = text.indexOf(expression, index + length);
    }
  } 
}