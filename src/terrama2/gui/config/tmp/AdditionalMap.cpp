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
  \file terrama2/gui/config/AdditionalMap.cpp

  \brief Definition of methods in class AdditionalMap.hpp

  \author Evandro Delatin
  \author Raphael Willian da Costa
  \author Carlos Augusto Teixeira Mendes
*/

/*!
\class AdditionalMap

It provides a object oriented wrapper over the data structures retrieved by analysis module

*/

// TerraMA2
#include "AdditionalMap.hpp"

// STL
#include <assert.h>

//! Constructor for new maps that it were not stored in database yet.
AdditionalMap::AdditionalMap()
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

//! Build and initialize a new monitored object with received data of analysis module 
AdditionalMap::AdditionalMap(const wsAdditionalMap& data)
{
  assert(data.id >= 0);
  _data = data;
}

//! Destructor
AdditionalMap::~AdditionalMap()
{
}

/*! \brief Change the identifier of a monitored object.

It must be used in only new maps that it has not been stored yet.

*/

void AdditionalMap::setId(int id)  
{
  assert(id >= 0);
  assert(isNew());
  _data.id = id;
}

//! Change the map creation date
void AdditionalMap::setCreationDate(QDate date)
{
  _data.creationYear  = date.year();
  _data.creationMonth = date.month();
  _data.creationDay   = date.day();
}

//! Change the map expiration date
void AdditionalMap::setExpirationDate(QDate date)
{
  _data.expirationYear  = date.year();
  _data.expirationMonth = date.month();
  _data.expirationDay   = date.day();
}

//! Change the theme parameters of raster type
void AdditionalMap::setThemeRasterParameters(int nLines, int nColumns, double resX, double resY)
{
  _data.baseTheme.nLines   = nLines;
  _data.baseTheme.nColumns = nColumns;
  _data.baseTheme.resX     = resX;
  _data.baseTheme.resY     = resY;
}
