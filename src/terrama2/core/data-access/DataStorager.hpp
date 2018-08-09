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
  \file terrama2/core/data-access/DataStorager.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_STORAGER_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_STORAGER_HPP__

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "../Config.hpp"
//TerraMA2
#include "../Shared.hpp"
#include "../data-access/DataSetSeries.hpp"
#include "../Config.hpp"
#include "../Shared.hpp"
#include "../Typedef.hpp"
#include "../data-model/DataManager.hpp"

namespace terrama2 {
namespace core {
struct DataSetSeries;
}  // namespace core
}  // namespace terrama2

namespace te
{
  namespace mem
  {
    class DataSet;
  } /* mem */
} /* te */

namespace terrama2
{
  namespace core
  {
    typedef std::string DataStoragerType;
    /*!
      \brief DataStorager provides an interface to store a DataSetSeries.
    */
    class TMCOREEXPORT DataStorager
    {
      public:
        /*!
          \brief The constructor stores the destination server information.
          \exception DataStoragerException Raised if the DataProvider is NULL
        */
        DataStorager(DataSeriesPtr outputDataSeries, DataProviderPtr outputDataProvider);
        //! Default destructor.
        virtual ~DataStorager() = default;

        DataStorager(const DataStorager& other) = delete;
        DataStorager(DataStorager&& other) = delete;
        DataStorager& operator=(const DataStorager& other) = delete;
        DataStorager& operator=(DataStorager&& other) = delete;

        virtual void store(const std::unordered_map<DataSetPtr,DataSetSeries >&  dataMap,
                           const std::vector< DataSetPtr >& dataSetLst,
                           const std::map<DataSetId, DataSetId>& inputOutputMap) const;

        /*!
          \brief Store the data series in outputDataSet.
        */
        virtual void store(DataSetSeries series, DataSetPtr outputDataSet) const = 0;

        virtual std::string getCompleteURI(DataSetPtr outputDataSet) const = 0;

      protected:
        DataSeriesPtr dataSeries_;
        DataProviderPtr dataProvider_;//!< Destination server information.
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_STORAGER_HPP__
