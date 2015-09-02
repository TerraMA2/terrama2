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
  \file terrama2/gui/config/WeatherGridList.hpp

  \brief Definition of methods in class WeatherGridList.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

#ifndef _WEATHERGRID_LIST_H_
#define _WEATHERGRID_LIST_H_

// QT
#include <QObject>
#include <QList>

// TerraMA2
#include "WeatherGrid.hpp"

class Services;
class ModColeta;

/*! \brief Classe responsável por manter e gerenciar a lista de grids 
           com dados climáticos disponíveis
*/           
class WeatherGridList : public QObject, public QList<WeatherGrid*>
{
Q_OBJECT

public:
  WeatherGridList(Services* manager, ModColeta* service);
  WeatherGridList(Services* manager, ModColeta* service, const wsWeatherDataSourceGeometry &geometry);
  ~WeatherGridList();
  
  bool         loadData();
  int          findGridIndex(int gridId) const;
  WeatherGrid* findGrid(int gridId) const;

  void reloadGridList();

  void setListChanged() { emit listChanged(); }

signals:
  void listChanged();

private:
  WeatherGridList(const WeatherGridList&);             //!< Remove construtor de cópia default
  WeatherGridList& operator=(const WeatherGridList&);  //!< Remove operador de atrib. default

  Services*   _manager; //! Gerenciador de serviços
  ModColeta*  _service; //! Referência para acesso ao serviço remoto de coleta de dados

  bool                         _filter;  //! Se for true, filtrar os grids pela geometria
  wsWeatherDataSourceGeometry  _geometry; //! Geometria do filtro
};


#endif

