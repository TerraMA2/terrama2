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
  \file terrama2/ws/collector/core/Collector.cpp

  \brief Class that represents a collector

  \author Gilberto Ribeiro de Queiroz, Paulo R. M. Oliveira
 */

// TerraMA2
#include "DatabaseStorageStrategy.hpp"
#include "Collector.hpp"


// terralib
#include <terralib/dataaccess.h>



terrama2::ws::collector::core::DatabaseStorageStrategy::DatabaseStorageStrategy(const std::string &storageName, const bool &uniqueStorage)
  : storageName_(storageName),
    uniqueStorage_(uniqueStorage)
{

}


terrama2::ws::collector::core::DatabaseStorageStrategy::~DatabaseStorageStrategy()
{

}

void terrama2::ws::collector::core::DatabaseStorageStrategy::store(std::shared_ptr<te::da::DataSet> dataSet, const terrama2::ws::collector::core::Collector &collector)
{

}
