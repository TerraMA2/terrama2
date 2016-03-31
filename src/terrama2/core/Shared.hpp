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
  \file terrama2/core/Shared.hpp

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_SHARED_HPP__
#define __TERRAMA2_CORE_SHARED_HPP__

#include <memory>

namespace terrama2 {
   namespace core {
      struct DataProvider;
      typedef std::shared_ptr<const terrama2::core::DataProvider> DataProviderPtr;

      struct DataSeries;
      typedef std::shared_ptr<const terrama2::core::DataSeries> DataSeriesPtr;

      struct DataSet;
      typedef std::shared_ptr<const terrama2::core::DataSet> DataSetPtr;
      struct DataSetDcp;
      typedef std::shared_ptr<const terrama2::core::DataSetDcp> DataSetDcpPtr;
      struct DataSetOccurrence;
      typedef std::shared_ptr<const terrama2::core::DataSetOccurrence> DataSetOccurrencePtr;
      struct DataSetGrid;
      typedef std::shared_ptr<const terrama2::core::DataSetGrid> DataSetGridPtr;

      class DcpSeries;
      typedef std::shared_ptr<terrama2::core::DcpSeries> DcpSeriesPtr;

      class GridSeries;
      typedef std::shared_ptr<terrama2::core::GridSeries> GridSeriesPtr;

      class OccurrenceSeries;
      typedef std::shared_ptr<terrama2::core::OccurrenceSeries> OccurrenceSeriesPtr;

      class MonitoredObjectSeries;
      typedef std::shared_ptr<terrama2::core::MonitoredObjectSeries> MonitoredObjectSeriesPtr;


      class DataAccessor;
      typedef std::shared_ptr<terrama2::core::DataAccessor> DataAccessorPtr;

      class DataRetriever;
      typedef std::shared_ptr<terrama2::core::DataRetriever> DataRetrieverPtr;
  }
}

#endif // __TERRAMA2_CORE_SHARED_HPP__
