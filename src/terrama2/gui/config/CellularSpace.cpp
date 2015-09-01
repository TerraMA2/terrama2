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
  \file terrama2/gui/config/CellularSpace.cpp

  \brief Definition of methods in class CellularSpace.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Gustavo Sampaio
*/

/*!
\class CellularSpace

Fornece um "wrapper" orientado a objetos sobre a estrutura de dados wsCellularSpace

*/

#include <assert.h>
#include "CellularSpace.hpp"
 
//! Construtor para novos espaços celulares, ainda não salvos na base de dados
CellularSpace::CellularSpace()
{
	_data.id = -1;  
	_data.baseTheme.id = -1;

	_data.baseTheme.projection.datum      = WS_DATUM_WGS84;
	_data.baseTheme.projection.hemisphere = WS_HEMISPHERE_SOUTH;
	_data.baseTheme.projection.lat0       = 0;
	_data.baseTheme.projection.lon0       = 0;
	_data.baseTheme.projection.name       = WS_PROJNAME_NoProjection;
	_data.baseTheme.projection.offx       = 0;
	_data.baseTheme.projection.offy       = 0;
	_data.baseTheme.projection.scale      = 0;
	_data.baseTheme.projection.stlat1     = 0;
	_data.baseTheme.projection.stlat2     = 0;
}
 
//! Constroi um novo espaço celular inicializado com dados recebidos do módulo de análises
CellularSpace::CellularSpace(const wsCellularSpace& data)
{
	assert(data.id >= 0);
	_data = data;
}

//! Destrutor
CellularSpace::~CellularSpace()
{
}
 
/*! \brief Altera o identificador de um espaço celular

	Deve ser utilizado apenas em espaços celulares novos que ainda não tenham
	sido salvos, para setar seu novo Id.
*/
void CellularSpace::setId(int id)  
{
	assert(id >= 0);
	assert(isNew());
	_data.id = id;
}
 
//! Altera a data de criação do espaço celular
void CellularSpace::setCreationDate(QDate date)
{
	_data.creationYear  = date.year();
	_data.creationMonth = date.month();
	_data.creationDay   = date.day();
}
 
//! Altera a data de validade do espaço celular
void CellularSpace::setExpirationDate(QDate date)
{
	_data.expirationYear  = date.year();
	_data.expirationMonth = date.month();
	_data.expirationDay   = date.day();
}

