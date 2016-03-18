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

#ifndef __TERRAMA2_CORE_DATA_ACCESS_OCCURRENCE_SERIES_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_OCCURRENCE_SERIES_HPP__

//TerraMA2
#include "../../Config.hpp"
#include "../data-model/DataSetOccurrence.hpp"

//STL
#include <vector>

//TerraLib
#include <terralib/memory/DataSet.h>

namespace terrama2
{
  namespace core
  {
    /*!
      \brief A DcpSeries represents a set of DCP.

      The DcpSeries aggregates the te::da::DataSet of each DCP
       as well as it's position and date-time column

    */
    class OccurrenceSeries
    {
    public:
      void addOccurrence(const std::shared_ptr<DataSetOccurrence>& dataset, std::shared_ptr<te::mem::DataSet>& memDataset) { datasetList_.emplace_back(dataset, memDataset);}
      const std::vector<std::pair<std::shared_ptr<DataSetOccurrence>, std::shared_ptr<te::mem::DataSet> > >& occurrenceList(){ return datasetList_; }

    private:
      std::vector<std::pair<std::shared_ptr<DataSetOccurrence>, std::shared_ptr<te::mem::DataSet> > > datasetList_;

    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_OCCURRENCE_SERIES_HPP__
