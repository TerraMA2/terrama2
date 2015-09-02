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
  \file terrama2/gui/config/Utils.cpp

  \brief Definition of Class Utils.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa  
  \author Carlos Augusto Teixeira Mendes
*/

// STL
#include <assert.h>

// TerraMA2
#include "Utils.hpp"

QString Utils::columnTypeToString(enum wsColumnType type)
{
  switch(type)
  {
  case WS_COLTYPE_DATETIME:    return QObject::tr("Data e hora");
    case WS_COLTYPE_STRING:      return QObject::tr("Texto");
    case WS_COLTYPE_REAL:        return QObject::tr("Real");
    case WS_COLTYPE_INT:         return QObject::tr("Inteiro");
	  case WS_COLTYPE_BLOB:        return QObject::tr("BLOB");
	  case WS_COLTYPE_OBJECT:      return QObject::tr("Objeto");
	  case WS_COLTYPE_CHARACTER:   return QObject::tr("Caracter");
	  case WS_COLTYPE_UNKNOWN:     return QObject::tr("Desconhecido");
	  case WS_COLTYPE_UNSIGNEDINT: return QObject::tr("Inteiro sem sinal");
    default:
      assert(0);
  }
  return ""; // Nunca chega aqui...
}

QString Utils::warningLevelToString(int warningLevel)
{
	switch(warningLevel)
	{
	case 0:	return QObject::tr("0 - Normal");
	case 1: return QObject::tr("1 - Observação");
	case 2: return QObject::tr("2 - Atenção");
	case 3: return QObject::tr("3 - Alerta");
	case 4: return QObject::tr("4 - Alerta Máximo");
	case 5: return QObject::tr("Nenhum");
	default:
		assert(0);
	}
	return ""; // Nunca chega aqui...
}

QString Utils::reportTypeToString(int reportType)
{
	switch(reportType)
	{
	case 0:	return QObject::tr("Simplificado");
	case 1: return QObject::tr("Completo");
	default:
		assert(0);
	}
	return ""; // Nunca chega aqui...
}

QPixmap Utils::warningLevelColor(int warningLevel, int width, int height)
{
	QPixmap itemPix = QPixmap(width,height);
	switch(warningLevel)
	{
	case 0:	itemPix.fill(Qt::transparent); break;
	case 1: itemPix.fill(Qt::blue); break;
	case 2: itemPix.fill(Qt::yellow); break;
	case 3: { QColor orange = QColor(255, 127, 0); itemPix.fill(orange); break; }
	case 4: itemPix.fill(Qt::red); break;
	case 5: itemPix.fill(Qt::transparent); break;
	default:
		assert(0);
	}
	return itemPix;
}
