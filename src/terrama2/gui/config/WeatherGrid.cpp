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
  \file terrama2/gui/config/WeatherGrid.cpp

  \brief Definition of methods in class WeatherGrid.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

/*!
\class WeatherGrid

Fornece um "wrapper" orientado a objetos sobre a estrutura de dados 
retornada pelo modulo de análise

*/

// QT
#include <assert.h>

// TerraMA2
#include "WeatherGrid.hpp"

//! Construtor para novos grids, ainda não salvos na base de dados
WeatherGrid::WeatherGrid()
{
  _data.id = -1;  
}

//! Constroi um novo grid com dados climáticos inicializado com dados recebidos do módulo de análises
WeatherGrid::WeatherGrid(const wsWeatherDataSource& data)
{
  assert(data.id >= 0);
  _data = data;
}

//! Destrutor
WeatherGrid::~WeatherGrid()
{
}

/*! \brief Altera o identificador de um grid com dados climáticos
Deve ser utilizado apenas em grids novos que ainda não tenham
sido salvos, para setar seu novo Id.
*/
void WeatherGrid::setId(int id)  
{
  assert(id >= 0);
  assert(isNew());
  _data.id = id;
}

//! Retorna número máximo de horas para o qual essa fonte possui bandas
int WeatherGrid::maxHours() 
{ 
	return floor((double)_data.grads_bandTimeOffset * (_data.grads_numBands - 1 ) / 60);
}

