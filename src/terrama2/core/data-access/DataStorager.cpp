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
  \file terrama2/core/data-access/DataStorager.cpp

  \brief

  \author Jano Simas
  \author Vinicius Campanha
 */

//TerraMA2
#include "DataStorager.hpp"
#include "../utility/Logger.hpp"
#include "../Exception.hpp"
#include "../../Config.hpp"

terrama2::core::DataStorager::DataStorager(DataProviderPtr outputDataProvider)
        : dataProvider_(outputDataProvider)
{
  if(!dataProvider_.get())
  {
    QString errMsg = QObject::tr("Mandatory parameters not provided.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw DataStoragerException() << ErrorDescription(errMsg);
  }
}

void terrama2::core::DataStorager::store(const std::unordered_map<DataSetPtr,DataSetSeries >&  dataMap,
                                         const std::vector< DataSetPtr >& dataSetLst,
                                         const std::map<DataSetId, DataSetId>& inputOutputMap) const
{
  for(auto& item : dataMap)
  {
    // store each item
    DataSetId outputDataSetId = inputOutputMap.at(item.first->id);
    auto outputDataSet = std::find_if(dataSetLst.cbegin(), dataSetLst.cend(), [outputDataSetId](terrama2::core::DataSetPtr dataSet) { return dataSet->id == outputDataSetId; });
    store(item.second, *outputDataSet);
  }
}
