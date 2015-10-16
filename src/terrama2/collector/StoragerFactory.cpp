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
  \file terrama2/collector/StoragerFactory.cpp

  \brief Instantiate storagers for DataProcessors.

  \author Jano Simas
*/

#include "StoragerFactory.hpp"
#include "StoragerPostgis.hpp"

#include "../core/DataSetItem.hpp"

terrama2::collector::StoragerPtr terrama2::collector::StoragerFactory::getStorager(terrama2::core::DataSetItem datasetItem)
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


  return StoragerPtr();
}