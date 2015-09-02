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
  \file terrama2/gui/config/LuaUtils.cpp

  \brief Definition of auxiliary functions for use with Lua

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

// TerraMA2
#include "LuaUtils.hpp"
#include <lua.hpp>

// QT
#include <QRegExp>

bool LuaUtils::checkSyntax(const char* script, QString& err, int& lin)
{
  lua_State* L = lua_open();
  
  bool ok = (luaL_loadstring(L, script) == 0);
  err = "";
  lin = 0;
  
  if(!ok)
  {
    QString msg = lua_tostring(L, -1);
    int pos = msg.indexOf(QRegExp("\\]:\\d+:"));  // Procura por padrão de formatação da string
    if(pos != -1) // Enconteramos. É um erro de sintaxe formatado segundo o que esperamos
    {
      QString line = msg.right(msg.count()-(pos+2));
      pos = line.indexOf(":");
      QString desc = line.right(line.count() - (pos+1));
      line.truncate(pos);
      lin  = line.toInt();
      
      err = QObject::tr("<p>Erro de sintaxe validando script.</p>"
                        "<p><b>Linha %1:</b></p>"
                        "<p>%2</p>").arg(lin).arg(desc);
    }
    else
      err = QObject::tr("<p>Erro validando script:</p><p>%1</p>").arg(msg);
  }
    
  lua_close(L);
  return ok;
}


