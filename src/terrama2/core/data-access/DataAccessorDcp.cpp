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
#include <terralib/memory/DataSet.h>

terrama2::core::DcpSeriesPtr terrama2::core::DataAccessorDcp::getDcpSeries(const Filter& filter)
{
  //if data provider is not active, nothing to do
  if(!dataProvider_.active)
  {
    //TODO: throw no data to collect
    //TODO: log this
  }

  DataRetrieverPtr dataRetriever = Factory::makeRetriever(dataProvider_);
  DcpSeriesPtr dcpSeries = std::make_shared<DcpSeries>();

  for(const auto& dataset : dataSeries_.datasetList)
  {
    //if the dataset is not active, continue to next.
    if(!dataset->active)
      continue;

    try
    {
      std::shared_ptr<DataSetDcp> datasetDcp = std::dynamic_pointer_cast<DataSetDcp>(dataset);

      // if this data retriever is a remote server that allows to retrieve data to a file,
      // download the file to a tmeporary location
      // if not, just get the DataProvider uri
      std::string uri;
      if(dataRetriever->isRetrivable())
        uri = retrieveData(dataRetriever, *datasetDcp, filter);
      else
        uri = dataProvider_.uri;

      //TODO: Set last date collected in filter
      std::shared_ptr<te::mem::DataSet> memDataSet = getDataSet(uri, filter, *datasetDcp);
      dcpSeries->addDcp(datasetDcp, memDataSet);

      //TODO: set last date collected
    }//try
    catch(const std::bad_cast& exp)
    {
      //TODO: log This
      continue;
    }//bad cast

  }//for each dataset

  return dcpSeries;
}

std::shared_ptr<te::da::DataSetTypeConverter> terrama2::core::DataAccessorDcp::getConverter( const DataSetDcp& datasetDcp, const std::shared_ptr<te::da::DataSetType>& datasetType) const
{
  std::shared_ptr<te::da::DataSetTypeConverter> converter(new te::da::DataSetTypeConverter(datasetType.get()));

  addColumns(converter, datasetType);

  adapt(datasetDcp, converter);
  converter->remove("FID");

  return converter;
}

void terrama2::core::DataAccessorDcp::addColumns(std::shared_ptr<te::da::DataSetTypeConverter> converter, const std::shared_ptr<te::da::DataSetType>& datasetType) const
{
  for(std::size_t i = 0, size = datasetType->size(); i < size; ++i)
  {
    te::dt::Property* p = datasetType->getProperty(i);

    converter->add(i,p->clone());
  }
}

te::dt::TimeInstantTZ terrama2::core::DataAccessorDcp::lastDateTime() const
{
  //TODO: implement lastDateTime
}
