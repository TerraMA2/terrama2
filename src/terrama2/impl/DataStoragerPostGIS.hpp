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
  \file terrama2/core/data-access/DataStoragerPostGIS.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_STORAGER_POSTGIS_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_STORAGER_POSTGIS_HPP__

//TerraMA2
#include "DataStoragerTable.hpp"

namespace terrama2
{
  namespace core
  {
    class DataStoragerPostGIS : public DataStoragerTable
    {
      public:
        DataStoragerPostGIS(DataSeriesPtr dataSeries, DataProviderPtr outputDataProvider)
                : DataStoragerTable(dataSeries, outputDataProvider) {}
        ~DataStoragerPostGIS() {}

        static DataStoragerPtr make(DataSeriesPtr dataSeries, DataProviderPtr dataProvider);
      protected:
        std::string getDataSetTableName(DataSetPtr dataSet) const;
        virtual std::string getCompleteURI(DataSetPtr outputDataSet) const override;

        virtual std::string driver() const override { return "POSTGIS"; }
        virtual std::string getDataSetName(DataSetPtr dataSet) const override { return getDataSetTableName(dataSet);}
    };

    class DataStoragerOccurrencePostGIS : public DataStoragerPostGIS
    {
      public:
        static DataStoragerType dataStoragerType() { return "OCCURRENCE-postgis"; }
    };

    class DataStoragerAnalysisMonitoredObject : public DataStoragerPostGIS
    {
      public:
        static DataStoragerType dataStoragerType() { return "ANALYSIS_MONITORED_OBJECT-postgis"; }
    };

    class DataStoragerWildfireEvent : public DataStoragerPostGIS
    {
      public:
        static DataStoragerType dataStoragerType() { return "GEOMETRIC_OBJECT-postgis"; }
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_STORAGER_POSTGIS_HPP__
