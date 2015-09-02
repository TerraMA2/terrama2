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
  \file terrama2/gui/config/WeatherGrid.hpp

  \brief Definition of methods in class WeatherGrid.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/


#ifndef _WEATHERGRID_H_
#define _WEATHERGRID_H_

// QT
#include <QString>

#include "soapModAnaliseProxy.h"

//! Classe responsável por representar uma classe de grades com informações climáticas
class WeatherGrid
{
public:
  WeatherGrid();
  WeatherGrid(const wsWeatherDataSource& data);
  ~WeatherGrid();

  //! Retorna o identificador do grid
  int id() const { return _data.id; }

  void setId(int id);  

  //! Retorna true se o grid for novo e ainda não salvo na base de dados
  bool isNew() const { return _data.id == -1; }
  
  //! Retorna true se a entrada for do tipo grid, false se for do tipo pontual
  wsWeatherDataSourceGeometry geometry() const { return _data.geometry; }
  
  //! Retorna o nome do grid
  QString name() const { return QString::fromStdString(_data.name); }

  //! Altera o nome do grid
  void setName(QString name) { _data.name = name.toStdString(); }

  //! Retorna uma descrição do grid
  QString description() const { return QString::fromStdString(_data.details); }

  //! Altera a descrição do grid
  void setDescription(QString description) { _data.details = description.toStdString(); }

  //! Retorna a estrutura que empacota os dados do grid
  const wsWeatherDataSource& data() const { return _data; }

  //! Retorna número máximo de horas para o qual essa fonte possui bandas
  int maxHours();

private:
  wsWeatherDataSource _data;  //! Dados do grid empacotados na estrutura lida / recebida do módulo de análises
};


#endif

