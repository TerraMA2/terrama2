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
  \file terrama2/core/data-access/DcpSeries.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DCP_SERIES_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DCP_SERIES_HPP__

//TerraMA2
#include "../../Config.hpp"
#include "../Shared.hpp"
#include "../data-model/DataSetDcp.hpp"
#include "../utility/Logger.hpp"
#include "SeriesAggregation.hpp"

//STL
#include <vector>

//TerraLib
#include <terralib/memory/DataSet.h>

//Qt
#include <QString>
#include <QObject>

namespace terrama2
{
  namespace core
  {
    /*!
      \brief A DcpSeries represents a set of DCP (Data Collecting Plataform).

      The DcpSeries aggregates the te::da::DataSet of each DCP

    */
    class DcpSeries : public SeriesAggregation
    {
    public:
      void addDcpSeries(std::map<DataSetPtr, Series > seriesMap)
      {
        dataSeriesMap_ = seriesMap;
        for(const auto& item : seriesMap)
        {
          try
          {
            DataSetDcpPtr dataset = std::dynamic_pointer_cast<const DataSetDcp>(item.first);
            dcpMap_.emplace(dataset, item.second);
          }
          catch(const std::bad_cast& exp)
          {
            QString errMsg = QObject::tr("Bad Cast to DataSetDcp");
            TERRAMA2_LOG_ERROR() << errMsg;
            continue;
          }//bad cast
        }
      }
      const std::map<DataSetDcpPtr, Series>& getDcpSeries(){ return dcpMap_; }

    private:
      std::map<DataSetDcpPtr, Series> dcpMap_;

    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DCP_SERIES_HPP__
