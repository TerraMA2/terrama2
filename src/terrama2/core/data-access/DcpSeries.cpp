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
//Qt
#include <QString>
#include <iostream>
#include <memory>
#include <utility>

#include "../Shared.hpp"
#include "../data-model/DataSetDcp.hpp"
#include "../utility/Logger.hpp"
//TerraMA2
#include "DcpSeries.hpp"

namespace terrama2 {
namespace core {
struct DataSetSeries;
}  // namespace core
}  // namespace terrama2


void terrama2::core::DcpSeries::addDcpSeries(std::unordered_map<DataSetPtr,DataSetSeries> seriesMap)
{
  dataSeriesMap_ = seriesMap;
  for(const auto& item : seriesMap)
  {
    try
    {
      DataSetDcpPtr dataset = std::dynamic_pointer_cast<const DataSetDcp>(item.first);
      dcpMap_.emplace(dataset, item.second);
    }
    catch(const std::bad_cast& exp)
    {
      QString errMsg = QObject::tr("Bad Cast to DataSetDcp");
      TERRAMA2_LOG_ERROR() << errMsg;
      continue;
    }//bad cast
  }
}

const std::unordered_map<terrama2::core::DataSetDcpPtr, terrama2::core::DataSetSeries>& terrama2::core::DcpSeries::dcpSeriesMap()
{
  return dcpMap_;
}
