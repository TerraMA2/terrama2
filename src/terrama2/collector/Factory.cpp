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

//TODO: review, should it be kept here?
#include "../core/ApplicationController.hpp"

#include "../core/DataManager.hpp"
#include "../core/DataSetItem.hpp"

#include "StoragerPostgis.hpp"
#include "ParserFirePoint.hpp"
#include "DataRetriever.hpp"
#include "DataRetrieverFTP.hpp"
#include "ParserPostgis.hpp"
#include "ParserOGR.hpp"
#include "Exception.hpp"
#include "Factory.hpp"

//std
#include <memory>
#include <algorithm>

//Qt
#include <QUrl>


terrama2::collector::ParserPtr terrama2::collector::Factory::makeParser(const std::string& uri, const terrama2::core::DataSetItem& datasetItem)
{
  QUrl url(uri.c_str());
  if(url.scheme().toLower() == "postgis")
  {
    ParserPtr newParser = std::make_shared<ParserPostgis>();
    return newParser;
  }

  if(url.scheme().toLower() == "file")
  {
    switch (datasetItem.kind()) {
      case core::DataSetItem::PCD_INPE_TYPE:
      case core::DataSetItem::PCD_TOA5_TYPE:
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
      default:
        assert(0);//TODO: throw, should not be here
        break;
    }
  }

  assert(0);//TODO: throw, should not be here
  return ParserPtr();
}

terrama2::collector::StoragerPtr terrama2::collector::Factory::makeStorager(const core::DataSetItem &datasetItem)
{
  std::map<std::string, std::string> storageMetadata = datasetItem.storageMetadata();

  if(storageMetadata.empty())
  {
    //TODO: review, should it be kept here?

    //If no storage metadata use standard storage as postgis in the same storage as TerraMA2 datamodel
    return std::make_shared<StoragerPostgis>(core::ApplicationController::getInstance().getDataSource());
  }


  //TODO: Exceptions

  std::map<std::string, std::string>::const_iterator localFind = storageMetadata.find("KIND");

  if(localFind == storageMetadata.cend())
    return StoragerPtr();

  std::string storagerKind = localFind->second;

  //to lower case
  std::transform(storagerKind.begin(), storagerKind.end(), storagerKind.begin(), ::tolower);
  if(storagerKind == "postgis")
  {
    return std::make_shared<StoragerPostgis>(storageMetadata);
  }


  //FIXME: throw here
  return StoragerPtr();
}

terrama2::collector::DataRetrieverPtr terrama2::collector::Factory::makeRetriever(const terrama2::core::DataProvider& dataProvider)
{
  switch (dataProvider.kind()) {
    case terrama2::core::DataProvider::FTP_TYPE:
      return std::make_shared<DataRetrieverFTP>(dataProvider);
      break;
    default:
      break;
  }
  return std::make_shared<DataRetriever>(dataProvider);
}
