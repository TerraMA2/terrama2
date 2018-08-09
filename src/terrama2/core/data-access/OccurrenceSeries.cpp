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
  \file terrama2/core/data-access/DcpSeries.cpp

  \brief

  \author Jano Simas
  \author Vinicius Campanha
 */

#include <QObject>
#include <QString>
#include <iostream>
#include <memory>
#include <utility>

#include "../Shared.hpp"
#include "../data-model/DataSetOccurrence.hpp"
#include "../utility/Logger.hpp"
//TerraMA2
#include "OccurrenceSeries.hpp"

namespace terrama2 {
namespace core {
struct DataSetSeries;
}  // namespace core
}  // namespace terrama2


void terrama2::core::OccurrenceSeries::addOccurrences(std::unordered_map<DataSetPtr,DataSetSeries> seriesMap)
{
  dataSeriesMap_ = seriesMap;
  for(const auto& item : seriesMap)
  {
    try
    {
      DataSetOccurrencePtr dataset = std::dynamic_pointer_cast<const DataSetOccurrence>(item.first);
      occurrenceMap.emplace(dataset, item.second);
    }
    catch(const std::bad_cast& exp)
    {
      QString errMsg = QObject::tr("Bad Cast to DataSetDcp");
      TERRAMA2_LOG_ERROR() << errMsg;
      continue;
    }//bad cast
  }
}

const std::unordered_map<terrama2::core::DataSetOccurrencePtr, terrama2::core::DataSetSeries>& terrama2::core::OccurrenceSeries::occurrencesMap()
{
  return occurrenceMap;
}
