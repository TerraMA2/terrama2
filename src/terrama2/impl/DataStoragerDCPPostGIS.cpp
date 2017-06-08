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
  \file terrama2/core/data-access/DataStoragerDCPPostGIS.cpp

  \brief

  \author Jano Simas
 */

//TerraMA2
#include "DataStoragerDCPPostGIS.hpp"

#include "../core/data-model/DataProvider.hpp"
#include "../core/utility/Raii.hpp"

#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/datasource/ScopedTransaction.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/core/uri/URI.h>

//Boost
#include <boost/algorithm/string.hpp>

void terrama2::core::DataStoragerDCPPostGIS::store(const std::unordered_map<DataSetPtr,DataSetSeries >&  dataMap,
                                                   const std::vector< DataSetPtr >& dataSetLst,
                                                   const std::map<DataSetId, DataSetId>& inputOutputMap) const
{

  { //connection scope
    te::core::URI uri(dataProvider_->uri);
    std::shared_ptr<te::da::DataSource> datasourceDestination(te::da::DataSourceFactory::make(driver(), uri));

    OpenClose< std::shared_ptr<te::da::DataSource> > openClose(datasourceDestination); Q_UNUSED(openClose);

    for(const auto& item : dataMap)
    {
      // read each item
      DataSetId outputDataSetId = inputOutputMap.at(item.first->id);
      auto outputDataSet = std::find_if(dataSetLst.cbegin(), dataSetLst.cend(), [outputDataSetId](terrama2::core::DataSetPtr dataSet) { return dataSet->id == outputDataSetId; });
      std::string destinationDataSetName = boost::to_lower_copy(getDataSetName(*outputDataSet));
    }

  }// end of the connection scope

  DataStorager::store(dataMap, dataSetLst, inputOutputMap);
}
