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

namespace terrama2
{
  namespace core
  {
    struct DataProvider;
    //! Shared smart pointer for DataProvider
    typedef std::shared_ptr<const terrama2::core::DataProvider> DataProviderPtr;

    struct DataSeries;
    //! Shared smart pointer for DataSeries
    typedef std::shared_ptr<const terrama2::core::DataSeries> DataSeriesPtr;

    struct DataSet;
    //! Shared smart pointer for DataSet
    typedef std::shared_ptr<const terrama2::core::DataSet> DataSetPtr;
    struct DataSetDcp;
    //! Shared smart pointer for DataSetDcp
    typedef std::shared_ptr<const terrama2::core::DataSetDcp> DataSetDcpPtr;
    struct DataSetOccurrence;
    //! Shared smart pointer for DataSetOccurrence
    typedef std::shared_ptr<const terrama2::core::DataSetOccurrence> DataSetOccurrencePtr;
    struct DataSetGrid;
    //! Shared smart pointer for DataSetGrid
    typedef std::shared_ptr<const terrama2::core::DataSetGrid> DataSetGridPtr;

    struct Process;
    //! Shared smart pointer for Process
    typedef std::shared_ptr<const terrama2::core::Process> ProcessPtr;

    class DcpSeries;
    //! Shared smart pointer for DcpSeries
    typedef std::shared_ptr<terrama2::core::DcpSeries> DcpSeriesPtr;
    class GridSeries;
    //! Shared smart pointer for GridSeries
    typedef std::shared_ptr<terrama2::core::GridSeries> GridSeriesPtr;
    class OccurrenceSeries;
    //! Shared smart pointer for OccurrenceSeries
    typedef std::shared_ptr<terrama2::core::OccurrenceSeries> OccurrenceSeriesPtr;
    class GeometricObjectSeries;
    //! Shared smart pointer for GeometricObjectSeries
    typedef std::shared_ptr<terrama2::core::GeometricObjectSeries> GeometricObjectSeriesPtr;

    class DataAccessor;
    //! Shared smart pointer for DataAccessor
    typedef std::shared_ptr<terrama2::core::DataAccessor> DataAccessorPtr;
    class DataRetriever;
    //! Shared smart pointer for DataRetriever
    typedef std::shared_ptr<terrama2::core::DataRetriever> DataRetrieverPtr;

    class DataStorager;
    //! Shared smart pointer for DataStorager
    typedef std::shared_ptr<terrama2::core::DataStorager> DataStoragerPtr;

    class SynchronizedDataSet;
    //! Shared smart pointer for SynchronizedDataSet
    typedef std::shared_ptr<terrama2::core::SynchronizedDataSet> SynchronizedDataSetPtr;

    class Timer;
    //! Shared smart pointer for Timer
    typedef std::shared_ptr<const terrama2::core::Timer> TimerPtr;

    class DataManager;
    //! Shared smart pointer for DataManager
    typedef std::shared_ptr<terrama2::core::DataManager> DataManagerPtr;

    class StoragerManager;
    //! Shared smart pointer for StoragerManager
    typedef std::shared_ptr<terrama2::core::StoragerManager> StoragerManagerPtr;

    struct Risk;
    //! Shared smart pointer for Legend
    typedef std::shared_ptr<const terrama2::core::Risk> LegendPtr;

    class Interpreter;
    using InterpreterPtr = std::shared_ptr<terrama2::core::Interpreter>;

    struct ReprocessingHistoricalData;
    typedef std::shared_ptr<terrama2::core::ReprocessingHistoricalData> ReprocessingHistoricalDataPtr;
  }
}

#endif // __TERRAMA2_CORE_SHARED_HPP__
