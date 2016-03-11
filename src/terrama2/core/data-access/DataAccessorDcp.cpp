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
  \file terrama2/core/data-access/DataAccessorDcp.cpp

  \brief

  \author Jano Simas
 */

//TerraMA2
#include "DataAccessorDcp.hpp"
#include "DataRetriever.hpp"
#include "../utility/Factory.hpp"

//TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>

DcpSeriesPtr terrama2::core::DataAccessorDcp::getDcpSeries(const Filter& filter)
{
  //if data provider is not active, nothing to do
  if(!dataProvider_.active)
  {
    //TODO: throw no data to collect
    //TODO: log this
  }

  DataRetrieverPtr dataRetriever = Factory::getDataRetriever(dataProvider_);
  DcpSeriesPtr dcpSeries = std::make_shared<DcpSeries>();

  for(const auto& datasetId : dataSeries_.datasetList)
  {
    DataSet dataset;//TODO: from datasetId

    //if the dataset is not active, continue to next.
    if(!dataset.active)
      continue;

    try
    {
      DataSetDcp& datasetDcp = dynamic_cast<DataSetDcp&>(dataset);

      te::common::uri::uri uri;
      // if this data retriever is a remote server that allows to retrieve data to a file,
      // download the file to a tmeporary location
      // if not, just get the DataProvider uri
      if(dataRetriever->isRetrivable())
        uri = retrieveData(dataRetriever, datasetDcp, filter);
      else
        uri = dataProvider_.uri;

      // creates a DataSource to the data and filters the dataset,
      // also joins if the DCP comes from separated files

      //te::da::DataSource local;
      //TODO:.. filter and join te::da::dataset from each dataset

      //TODO:add each Dcp to a DcpSeriesPtr
    }//try
    catch(const std::bad_cast& exp)
    {
      //TODO: log This
      continue;
    }//bad cast

  }//for each dataset

  return dcpSeries;
}
