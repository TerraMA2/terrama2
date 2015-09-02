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
  \file terrama2/gui/config/RiskMap.cpp

  \brief Definition of methods in class RiskMap.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

/*!
\class RiskMap

Fornece um "wrapper" orientado a objetos sobre a estrutura de dados 
retornada pelo modulo de análise

*/


//STL
#include <assert.h>

// TerraMA2
#include "RiskMap.hpp"

/*
struct wsProjectionParams
{
	enum wsProjectionName name;
	enum wsDatum          datum;
	int                   zone;
	double                lon0;
	double                lat0;
	double                offx;
	double                offy;
	double                stlat1;
	double                stlat2;
	std::string           units;
	double                scale;
	enum wsHemisphere     hemisphere;
};
*/

//! Construtor para novos mapas, ainda não salvos na base de dados
RiskMap::RiskMap()
{
  _data.id = -1;  
  _data.baseRiskMapTheme.id = -1;

  _data.baseRiskMapTheme.projection.datum      = WS_DATUM_WGS84;
  _data.baseRiskMapTheme.projection.hemisphere = WS_HEMISPHERE_SOUTH;
  _data.baseRiskMapTheme.projection.lat0       = 0;
  _data.baseRiskMapTheme.projection.lon0       = 0;
  _data.baseRiskMapTheme.projection.name       = WS_PROJNAME_NoProjection;
  _data.baseRiskMapTheme.projection.offx       = 0;
  _data.baseRiskMapTheme.projection.offy       = 0;
  _data.baseRiskMapTheme.projection.scale      = 0;
  _data.baseRiskMapTheme.projection.stlat1     = 0;
  _data.baseRiskMapTheme.projection.stlat2     = 0;
}

//! Constroi um novo objeto monitorado inicializado com dados recebidos do módulo de análises
RiskMap::RiskMap(const wsRiskMap& data)
{
  assert(data.id >= 0);
  _data = data;
}

//! Destrutor
RiskMap::~RiskMap()
{
}

/*! \brief Altera o identificador de um objeto monitorado

Deve ser utilizado apenas em mapas novos que ainda não tenham
sido salvas, para setar seu novo Id.
*/
void RiskMap::setId(int id)  
{
  assert(id >= 0);
  assert(isNew());
  _data.id = id;
}

//! Altera a data de criação do mapa
void RiskMap::setCreationDate(QDate date)
{
  _data.creationYear  = date.year();
  _data.creationMonth = date.month();
  _data.creationDay   = date.day();
}

//! Altera a data de validade do mapa
void RiskMap::setExpirationDate(QDate date)
{
  _data.expirationYear  = date.year();
  _data.expirationMonth = date.month();
  _data.expirationDay   = date.day();
}

