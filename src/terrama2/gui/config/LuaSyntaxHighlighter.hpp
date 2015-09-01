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
  \file terrama2/gui/config/LuaSyntaxHighlighter.hpp

  \brief Definition of Class LuaSyntaxHighlighter.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/


#ifndef _LUA_SYNTAX_HIGHLIGHTER_H_
#define _LUA_SYNTAX_HIGHLIGHTER_H_

// QT
#include <QSyntaxHighlighter>
#include <QRegExp>
#include <QTextCharFormat>
#include <QVector>

//! Classe responsável por efetuar o highlight de scripts Lua
class LuaSyntaxHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT

public:
  LuaSyntaxHighlighter(QTextEdit* parent = 0);

protected:
  void highlightBlock(const QString &text);

private:
  struct HighlightingRule
  {
    QRegExp         _pattern;
    QTextCharFormat _format;
  };
  
  QVector<HighlightingRule> _rules;
};


#endif

