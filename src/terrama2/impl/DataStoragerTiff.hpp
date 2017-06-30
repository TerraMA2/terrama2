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
  \file terrama2/core/data-access/DataStoragerTiff.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_STORAGER_TIF_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_STORAGER_TIF_HPP__

//TerraMA2
#include "../core/data-access/DataStorager.hpp"
#include "../core/utility/Logger.hpp"

//QT
#include <QString>
#include <QObject>

//Terralib
#include <terralib/datatype/TimeInstantTZ.h>

namespace terrama2
{
  namespace core
  {
    /*!
      \brief DataStoragerTiff Class to create raster tif files.
    */
    class DataStoragerTiff : public DataStorager
    {
      public:
        DataStoragerTiff(DataSeriesPtr dataSeries, DataProviderPtr outputDataProvider)
                : DataStorager(dataSeries, outputDataProvider) {}
        ~DataStoragerTiff() {}

        static DataStoragerPtr make(DataSeriesPtr dataSeries, DataProviderPtr dataProvider);
        static DataStoragerType dataStoragerType() { return "GRID-geotiff"; }

        /*!
          \brief Create a raster file for each raster in the series.

          The mask will be replaced by the timestamp in the timestamp attribute.

          The file extension should be '.tif', if not it'll append '.tif' at the and of the filename.
        */
        virtual void store(DataSetSeries series, DataSetPtr outputDataSet) const override;

        /*!
          \brief Return the complete uri of the dataset.

          The uri is composed by the dataProvider_, the dataset folder and the dataset mask.
        */
        virtual std::string getCompleteURI(DataSetPtr outputDataSet) const override;

      protected:
        int getSRID(DataSetPtr dataSet, bool log) const;
        std::string getTimezone(DataSetPtr dataSet, bool logError = true) const;
        std::string zeroPadNumber(long num, int size) const;
        std::string replaceMask(const std::string& mask,
                                std::shared_ptr<te::dt::DateTime> timestamp,
                                terrama2::core::DataSetPtr dataSet) const;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_STORAGER_TIF_HPP__
