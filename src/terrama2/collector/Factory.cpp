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

#include "../core/DataManager.hpp"
#include "../core/DataSetItem.hpp"

#include "StoragerPostgis.hpp"
#include "CollectorFile.hpp"
#include "DataRetriever.hpp"
#include "ParserPostgis.hpp"
#include "ParserOGR.hpp"
#include "Exception.hpp"
#include "Factory.hpp"

//std
#include <memory>
#include <algorithm>

//Qt
#include <QUrl>


terrama2::collector::CollectorPtr terrama2::collector::Factory::getCollector(uint64_t dataProviderId)
{
  //If there is no collector for this DataProvider, create one.
  if(!collectorMap_.contains(dataProviderId))
  {
    core::DataProvider provider = core::DataManager::getInstance().findDataProvider(dataProviderId);
    //... instatiate a new collector
    //TODO: Throws if fail?

    //TODO: Throws UnknownDataProviderKindException
    //TODO: Use URI scheme to switch?
    switch (provider.kind()) {
      case core::DataProvider::FILE_TYPE:
      {
        CollectorPtr newCollector = std::make_shared<CollectorFile>(provider);
        collectorMap_.insert(dataProviderId, newCollector);
        break;
      }
      default:
        break;
    }
  }

  return collectorMap_.value(dataProviderId);
}

void terrama2::collector::Factory::removeCollector(uint64_t dataProviderId)
{
  collectorMap_.remove(dataProviderId);
}

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
      case core::DataSetItem::FIRE_POINTS_TYPE:
      {
        ParserPtr newParser = std::make_shared<ParserOGR>();
        return newParser;
      }
      default:
        break;
    }
  }

  //FIXME: throw here
  return ParserPtr();
}

terrama2::collector::StoragerPtr terrama2::collector::Factory::makeStorager(const core::DataSetItem &datasetItem)
{
  std::map<std::string, std::string> storageMetadata = datasetItem.storageMetadata();

  //Exceptions

  std::string storagerKind = storageMetadata.at("KIND");
  if(storagerKind.empty())
    return StoragerPtr();

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
  return std::make_shared<DataRetriever>(dataProvider);
}
