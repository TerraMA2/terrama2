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
  \file src/terrama2/services/maps/core/JSonUtils.cpp

  \brief Methods to convertion between a Maps and JSon object

  \author Vinicius Campanha
*/

// TerraMA2
#include "JSonUtils.hpp"
#include "../../../core/Exception.hpp"
#include "../../../core/utility/JSonUtils.hpp"
#include "../../../core/utility/Logger.hpp"

// Terralib
#include <terralib/geometry/WKTReader.h>

// Qt
#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>

terrama2::services::maps::core::MapsPtr terrama2::services::maps::core::fromMapsJson(QJsonObject json)
{
  if(json["class"].toString() != "Maps")
  {
    QString errMsg = QObject::tr("Invalid Maps JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  // VINICIUS:
  if(!json.contains("id"))
  {
    QString errMsg = QObject::tr("Invalid Maps JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  terrama2::services::maps::core::Maps* map = new terrama2::services::maps::core::Maps();
  terrama2::services::maps::core::MapsPtr mapsPtr(map);

  return mapsPtr;
}


QJsonObject terrama2::services::maps::core::toJson(MapsPtr maps)
{
  // VINICIUS:
  QJsonObject obj;
  obj.insert("class", QString("Maps"));


  return obj;
}

