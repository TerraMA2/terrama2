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
  \file terrama2/gui/config/WeatherGridList.cpp

  \brief Definition of methods in class WeatherGridList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

/*!
\class WeatherGridList

É a classe responsável por efetuar a comunicação com o
módulo de análises.

Herda de QList para facilitar a consulta aos dados, porém 
adição e remoção de grids devem ser feitas através da 
API própria para garantir a sincronização com o servidor.
*/

// STL
#include <assert.h>

// TerraMA2
#include "WeatherGridList.hpp"
#include "Services.hpp"

//! Construtor
WeatherGridList::WeatherGridList(Services* manager, ModColeta* service)
{
  assert(manager && service);
  _manager = manager;
  _service = service;

  _filter  = false;
}

WeatherGridList::WeatherGridList(Services* manager, ModColeta* service, const wsWeatherDataSourceGeometry &geometry)
{
  assert(manager && service);
  _manager = manager;
  _service = service;

  _filter   = true;
  _geometry = geometry;
}

//! Destrutor
WeatherGridList::~WeatherGridList()
{
}

/*! \brief Carrega lista remota de servidores do módulo de coleta  

Retorna true se conseguiu carregar a lista.  Caso contrário, 
MOSTRA mensagem de erro e retorna false. 

*/
bool WeatherGridList::loadData()
{
  // Efetua chamada remota
#ifdef OFFLINE_TEST
  static wsPCD pcd_colPts[] = { { "filename1.txt", -47.1021, -23.1231  },
                                { "filename2.txt", -46.7021, -21.2411  },
                                { "filename3.txt", -45.1351, -24.3251  },
                                { "filename4.txt", -47.2441, -23.2337  } };
  std::vector<wsPCD> vectpcd_colPts(pcd_colPts, pcd_colPts + sizeof(pcd_colPts)/sizeof(wsPCD));
  static wsWeatherDataSource testData[] = {
    {0, 1, WS_WDSGEOM_RASTER, "Grid Clima 1", WS_WDSTYPE_PREDICTED_RAIN,   {}, 10.0, 1.0, "Details 1", "Path 1", "Mask 1", WS_WDSFFMT_ASCII_Grid, WS_ASCIIGRIDCOORD_DECIMAL_DEGREES, vectpcd_colPts, "script01.lua", "nav01.nav", "cltfile01.ctl", WS_GRADSDATATYPE_INTEGER16, 1},
    {1, 1, WS_WDSGEOM_RASTER, "Grid Clima 2", WS_WDSTYPE_ACCUMULATED_RAIN, {}, 11.0, 2.0, "Details 2", "Path 2", "Mask 2", WS_WDSFFMT_PCD,        WS_ASCIIGRIDCOORD_MILLIDEGREES,    vectpcd_colPts, "script01.lua", "nav02.nav", "cltfile02.ctl", WS_GRADSDATATYPE_FLOAT32,   2},
    {2, 0, WS_WDSGEOM_RASTER, "Grid Clima 3", WS_WDSTYPE_OTHER,            {}, 12.0, 3.0, "Details 3", "Path 3", "Mask 3", WS_WDSFFMT_TIFF,       WS_ASCIIGRIDCOORD_DECIMAL_DEGREES, vectpcd_colPts, "script01.lua", "nav03.nav", "cltfile03.ctl", WS_GRADSDATATYPE_INTEGER16, 3},
    {3, 0, WS_WDSGEOM_RASTER, "Grid Clima 4", WS_WDSTYPE_PREDICTED_RAIN,   {}, 13.0, 4.0, "Details 4", "Path 4", "Mask 4", WS_WDSFFMT_GrADS,      WS_ASCIIGRIDCOORD_MILLIDEGREES,    vectpcd_colPts, "script01.lua", "nav04.nav", "cltfile04.ctl", WS_GRADSDATATYPE_FLOAT32,   4},
    {4, 1, WS_WDSGEOM_RASTER, "Grid Clima 5", WS_WDSTYPE_PREDICTED_RAIN,   {}, 14.0, 5.0, "Details 5", "Path 5", "Mask 5", WS_WDSFFMT_ASCII_Grid, WS_ASCIIGRIDCOORD_DECIMAL_DEGREES, vectpcd_colPts, "script01.lua", "nav05.nav", "cltfile05.ctl", WS_GRADSDATATYPE_INTEGER16, 5},
  };
  std::vector<wsWeatherDataSource> result(testData, testData + sizeof(testData)/sizeof(wsWeatherDataSource));
#else
  std::vector<struct wsWeatherDataSource> result;
  if(_service->collection__getWeatherDataSourceList(result) != SOAP_OK)
    return _manager->showModColetaError(QObject::tr("Não foi possível carregar a lista de servidores."));
#endif

  // Cria objetos do tipo weatherGrid para cada grid retornado
  for(int i=0, count=(int)result.size(); i<count; i++)
  {
    if(_filter && _geometry != result[i].geometry)
      continue;
    WeatherGrid* map = new WeatherGrid(result[i]);
    append(map);
  }
  return true;
}

/*! \brief Retorna o índice do grid associado com um identificador

Retorna -1 se o identificador não tiver sido encontrado
*/
int WeatherGridList::findGridIndex(int gridId) const
{
  for(int i=0, num=(int)count(); i<num; i++)
  {
    if(at(i)->id() == gridId)
      return i;
  }
  return -1;
}

/*! \brief Retorna o grid associado com um identificador

Retorna NULL se o identificador não tiver sido encontrado
*/
WeatherGrid* WeatherGridList::findGrid(int gridId) const
{
  int index = findGridIndex(gridId);
  return (index != -1) ? at(index) : NULL;
}

void WeatherGridList::reloadGridList()
{
  clear();
  loadData();
}
