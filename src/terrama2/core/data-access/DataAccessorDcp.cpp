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

DcpSeriesPtr DataAccessorDcp::getDcpSeries(DataFilter)
{
  DataRetrieverPtr dataRetriever = getDataRetriever(DataProvider);
  DcpSeriesPtr dcpSeries = std::make_shared<DcpSeries>();

  for(const auto& dataset : dataSeries_)
  {
    try
    {
      const DataSetDcp& datasetDcp = dynamic_cast<const DataSetDcp&>(dataset);

      te::core::URI uri;
      if(dataRetriever->isRetrivable())
        uri = retrieveData(dataRetriever, datasetDcp, Filter);
      else
        uri = DataProvider.uri;

        te::da::DataSource local;
        //.. filter and join te::da::dataset from each dataset

        te::gm::Point position = datasetDcp.position();
        //add each Dcp to a DcpSeriesPtr
    }//try
    catch(const std::bad_cast& exp)
    {
      //TODO: log This
      continue;
    }//bad cast

  }//for each dataset

  return dcpSeries;
}
