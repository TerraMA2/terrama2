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
  \file terrama2/collector/TransferenceData.hpp

  \brief Structure to tranfere data during collection.

  \author Jano Simas
*/


#ifndef __TERRAMA2_COLLECTOR_TRANFERENCEDATA_HPP__
#define __TERRAMA2_COLLECTOR_TRANFERENCEDATA_HPP__

#include "../core/DataSet.hpp"
#include "../core/DataSetItem.hpp"

//STL
#include <string>
#include <memory>

//Terralib
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetType.h>

namespace terrama2
{
  namespace collector
  {
    struct TransferenceData
    {
      core::DataSet dataSet;
      core::DataSetItem dataSetItem;

      std::string uriOrigin;//!< Uri of the original data.
      std::string uriTemporary;//!< Uri where the data is stored temporary, before final store.
      std::string uriStorage;//!< Uri where the data storage.

      std::shared_ptr< te::dt::TimeInstantTZ > dateData;//! Date/Time when data was produced.
      std::shared_ptr< te::dt::TimeInstantTZ > dateCollect;//! Date/Time when data was collected.

      std::shared_ptr<te::da::DataSet> teDataSet;
      std::shared_ptr<te::da::DataSetType> teDataSetType;
    };
  }
}

#endif //TRANFERENCEDATA_
