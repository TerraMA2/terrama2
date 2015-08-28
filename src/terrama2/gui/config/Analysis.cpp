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
  \file terrama2/gui/config/Analysis.cpp

  \brief Definition of methods in class Analysis.hpp

  \author Evandro Delatin

  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

/*!
\class Analysis

It provides a object orientation wrapper over data structure retrieved by analysis module.

*/

// TerraMA2
#include "Analysis.hpp"
#include "riskMap.h"
#include "cellularSpace.h"
#include "weatherGrid.h"
#include "view.h"

// STL
#include <assert.h>

//! The Constructor for new analysis that it has not been saved in database
Analysis::Analysis()
{
  _data.id = -1;  
}

//! Build an new analysis initialized with received data of analysis module
Analysis::Analysis(const wsAnalysisRule& data)
{
  assert(data.id >= 0);
  _data = data;
}

//! Destructor
Analysis::~Analysis()
{
}

/*! \brief Change the identifier of analysis role.

It must be used in new roles only that have not been saved yet and then set the new Identifier

*/
void Analysis::setId(int id)  
{
  assert(id >= 0);
  assert(isNew());
  _data.id = id;
}

/*! \brief Change the monitored object data associated with an analyse role.

Make an copy of received data

*/
void Analysis::setRiskMap(const RiskMap& map) 
{ 
  _data.riskMap = map.data(); 
}

/*! \brief Change the PCD data associated with an analyse role.

Make an copy of received data

*/
void Analysis::setPCD(const WeatherGrid& pcd)
{
  _data.pcd = pcd.data();
}

/*! \brief Change the sight data associated with an analyse role

Make an copy of received data

*/
void Analysis::setView(const View& view)
{
  _data.pcdView = view.data();
}

/*! \brief Change the "cell space" associated with an analyse role

Make an copy of received data

*/
void Analysis::setCellularSpace(const CellularSpace& cs)
{
	_data.cellularSpace = cs.data();
}

/*! \brief Change the wheather informations of grid data with an analyse role.

Make an copy of received data

*/
void Analysis::setWeatherGrids(const QList<WeatherGrid*>& gridlist)
{
  _data.inputWeatherDataSources.clear();
  for(int i=0,count=(int)gridlist.size(); i<count; i++)
    _data.inputWeatherDataSources.push_back(gridlist[i]->data());
}

QList<wsWeatherDataSource*> Analysis::getWeatherGrids()
{
  QList<wsWeatherDataSource*> weatherGrids;
  for (int i=0; i < _data.inputWeatherDataSources.size(); i++)
  {
    weatherGrids.append(&_data.inputWeatherDataSources.at(i));
  }
  return weatherGrids;
}