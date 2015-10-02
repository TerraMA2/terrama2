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
  \file terrama2/collector/CollectorFactory.cpp

  \brief Instantiate collectors for DataProviders.

  \author Jano Simas
*/


#include "CollectorFactory.hpp"
#include "CollectorFile.hpp"
#include "Exception.hpp"

terrama2::collector::CollectorPtr terrama2::collector::CollectorFactory::getCollector(const core::DataProviderPtr dataProvider)
{
  //JANO: implementar getCollector

  //If there is no collector for this DataProvider, create one.
  if(!collectorMap_.contains(dataProvider->id()))
  {
    //... instatiate a new collector
    //TODO: Throws if fail?

    //TODO: Throws UnknownDataProviderKindException
    switch (dataProvider->kind()) {
      case core::DataProvider::FILE_TYPE:
      {
        CollectorPtr newCollector(new CollectorFile(dataProvider));
        collectorMap_.insert(dataProvider->id(), newCollector);
        break;
      }
      default:
        break;
    }
  }

  return collectorMap_.value(dataProvider->id());
}

void terrama2::collector::CollectorFactory::removeCollector(const terrama2::core::DataProviderPtr dataProvider)
{
  collectorMap_.remove(dataProvider->id());
}
