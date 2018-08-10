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
  \file terrama2/core/utility/StoragerManager.hpp
  \brief
  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_CORE_STORAGER_MANAGER_HPP__
#define __TERRAMA2_CORE_STORAGER_MANAGER_HPP__

#include <condition_variable>
#include <map>
#include <mutex>
#include <string>
// STL
#include <vector>

#include "../Shared.hpp"
// TerraMa2
#include "../Typedef.hpp"
#include "../Config.hpp"
#include "../Shared.hpp"
#include "../data-access/DataSetSeries.hpp"

namespace terrama2 {
namespace core {
struct DataSetSeries;
}  // namespace core
}  // namespace terrama2

namespace terrama2
{
  namespace core
  {
    class TMCOREEXPORT StoragerManager
    {
      public:
        StoragerManager(terrama2::core::DataManagerPtr dataManager);

        void store(DataSetSeries series, DataSetPtr outputDataSet);

        void removeUriFromQueue(const std::string& uri);

      private:
        terrama2::core::DataManagerPtr dataManager_;
        std::vector<std::string> vecURIs_;
        std::mutex mutex_;
        std::condition_variable conditionVariable_;
    };


  }
}

#endif //__TERRAMA2_CORE_STORAGER_MANAGER_HPP__
