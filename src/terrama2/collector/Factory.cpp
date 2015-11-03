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
#include "Exception.hpp"
#include "ParserOGR.hpp"
#include "Factory.hpp"


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
        CollectorPtr newCollector(new CollectorFile(provider));
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

terrama2::collector::ParserPtr terrama2::collector::Factory::getParser(terrama2::core::DataSetItem::Kind datasetItemKind)
{
  switch (datasetItemKind) {
    case core::DataSetItem::PCD_INPE_TYPE:
    case core::DataSetItem::PCD_TOA5_TYPE:
    {
      ParserPtr newParser(new ParserOGR());
      return newParser;
    }

    default:
      break;
  }

  //FIXME: throw here
  return ParserPtr();
}

terrama2::collector::StoragerPtr terrama2::collector::Factory::getStorager(terrama2::core::DataSetItem datasetItem)
{
  std::map<std::string, std::string> storageMetadata = datasetItem.storageMetadata();

  //Exceptions

  std::string storagerKind = storageMetadata.at("KIND");
  if(storagerKind.empty())
    return StoragerPtr();

  if(storagerKind == "POSTGIS")
  {
    return StoragerPtr(new StoragerPostgis(storageMetadata));
  }


  //FIXME: throw here
  return StoragerPtr();
}
