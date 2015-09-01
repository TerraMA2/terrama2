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
  \file terrama2/gui/config/RemoteWeatherServerData.cpp

  \brief Definition of methods in class RemoteWeatherServerData.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Celso Luiz Ramos Cruz
*/


/*!
\class RemoteWeatherServerData

Fornece um "wrapper" orientado a objetos sobre a estrutura de dados 
retornada pelo modulo de análise

*/

// STL
#include <assert.h>

// TerraMA2
#include "RemoteWeatherServerData.hpp"

//! Construtor para novas análises, ainda não salvas na base de dados
RemoteWeatherServerData::RemoteWeatherServerData()
{
  _data.id = -1;  

  _data.projection.datum      = WS_DATUM_WGS84;
  _data.projection.hemisphere = WS_HEMISPHERE_SOUTH;
  _data.projection.lat0       = 0;
  _data.projection.lon0       = 0;
  _data.projection.name       = WS_PROJNAME_NoProjection;
  _data.projection.offx       = 0;
  _data.projection.offy       = 0;
  _data.projection.scale      = 0;
  _data.projection.stlat1     = 0;
  _data.projection.stlat2     = 0;

}

//! Constroi uma nova análise inicializada com dados recebidos do módulo de análises
RemoteWeatherServerData::RemoteWeatherServerData(const wsWeatherDataSource& data)
{
  assert(data.id >= 0);
  _data = data;
}

//! Destrutor
RemoteWeatherServerData::~RemoteWeatherServerData()
{
}

/*! \brief Altera o identificador de uma regra de análise.

Deve ser utilizado apenas em regras novas que ainda não tinham
sido salvas, para setar seu novo Id.
*/
void RemoteWeatherServerData::setId(int id)  
{
  assert(id >= 0);
  assert(isNew());
  _data.id = id;
}
