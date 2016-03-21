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
  \file terrama2/collector/Factory.cpp

  \brief Instantiate collectors .

  \author Jano Simas
*/

#include "../core/DataSetItem.hpp"
#include "../core/Logger.hpp"

#include "Exception.hpp"
#include "Factory.hpp"

#include "StoragerPostgis.hpp"
#include "StoragerTiff.hpp"

#include "ParserFirePoint.hpp"
#include "ParserPcdInpe.hpp"
#include "ParserPcdToa5.hpp"
#include "ParserPostgis.hpp"
#include "ParserOGR.hpp"
#include "ParserTiff.hpp"
#include "ParserAscGrid.hpp"

#include "DataRetriever.hpp"
#include "DataRetrieverFTP.hpp"

//std
#include <memory>
#include <algorithm>

//Qt
#include <QUrl>

terrama2::collector::ParserPtr terrama2::collector::Factory::makeParser(const terrama2::core::DataSetItem& datasetItem)
{
  switch (datasetItem.kind()) {
    case core::DataSetItem::PCD_INPE_TYPE:
    {
      ParserPtr newParser = std::make_shared<ParserPcdInpe>();
      return newParser;
    }
    case core::DataSetItem::PCD_TOA5_TYPE:
    {
      ParserPtr newParser = std::make_shared<ParserPcdToa5>();
      return newParser;
    }
    case core::DataSetItem::UNKNOWN_TYPE:
    {
      ParserPtr newParser = std::make_shared<ParserOGR>();
      return newParser;
    }
    case core::DataSetItem::FIRE_POINTS_TYPE:
    {
      ParserPtr newParser = std::make_shared<ParserFirePoint>();
      return newParser;
    }
    case core::DataSetItem::GRID_TYPE:
    {
      ParserPtr newParser = std::make_shared<ParserTiff>();
      return newParser;
    }
    case core::DataSetItem::STATIC_DATA:
    {
      ParserPtr newParser = std::make_shared<ParserOGR>();
      return newParser;
    }
    default:
    {
      QString errMsg = QObject::tr("The DataSetItem (%1) type is not compatible with FILE scheme.").arg(datasetItem.id());
      TERRAMA2_LOG_ERROR() << errMsg;
      throw ConflictingParserTypeSchemeException() << terrama2::ErrorDescription(errMsg);
    }
  }

  //FIXME: define new type of dataset to postgis data!!!
  ParserPtr newParser = std::make_shared<ParserPostgis>();
  return newParser;

  QString errMsg = QObject::tr("Unknown  DataSetItem (%1) type.").arg(datasetItem.id());
  TERRAMA2_LOG_ERROR() << errMsg;
  throw UnableToCreateParserException() << terrama2::ErrorDescription(errMsg);
}

terrama2::collector::StoragerPtr terrama2::collector::Factory::makeStorager(const core::DataSetItem &datasetItem)
{
  std::map<std::string, std::string> metadata = datasetItem.metadata();
  std::map<std::string, std::string>::const_iterator localFind = metadata.find("KIND");

  if(localFind == metadata.cend())
  {
    QString errMsg = QObject::tr("No storager kind set.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UnableToCreateStoragerException() << terrama2::ErrorDescription(errMsg);
  }

  std::string storagerKind = localFind->second;

  //to lower case
  std::transform(storagerKind.begin(), storagerKind.end(), storagerKind.begin(), ::toupper);
  if(storagerKind == "POSTGIS")
  {
    return std::make_shared<StoragerPostgis>(metadata);
  }
  else if(storagerKind == "TIFF")
  {
    return std::make_shared<StoragerTiff>(metadata);
  }

  QString errMsg = QObject::tr("Unknown  DataSetItem (%1) type.").arg(datasetItem.id());
  TERRAMA2_LOG_ERROR() << errMsg;
  throw UnableToCreateStoragerException() << terrama2::ErrorDescription(errMsg);
}

terrama2::collector::DataRetrieverPtr terrama2::collector::Factory::makeRetriever(const terrama2::core::DataProvider& dataProvider)
{
  switch (dataProvider.kind()) {
    case terrama2::core::DataProvider::FTP_TYPE:
      return std::make_shared<DataRetrieverFTP>(dataProvider);
    default:
      break;
  }
  //Dummy retriever
  return std::make_shared<DataRetriever>(dataProvider);
}
