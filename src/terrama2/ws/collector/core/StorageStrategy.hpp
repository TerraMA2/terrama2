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
  \file terrama2/ws/collector/core/StorageStrategy.hpp

  \brief Abstract class to define a storage strategy

  \author Paulo R. M. Oliveira
 */

#ifndef __TERRAMA2_WS_COLLECTOR_CORE_STORAGE_STRATEGY_HPP__
#define __TERRAMA2_WS_COLLECTOR_CORE_STORAGE_STRATEGY_HPP__


#include "Collector.hpp"

// STL
#include <memory>
#include <string>

// QT
#include <QJsonObject>

// terralib
#include <terralib/dataaccess/dataset/DataSet.h>

namespace terrama2
{
  namespace ws
  {
    namespace collector
    {
      namespace core
      {

        class StorageStrategy
        {
        public:

          /*!
          * \brief Constructor.
          */
          StorageStrategy();

          /*!
          * \brief Destructor.
          */
          virtual ~StorageStrategy();

          /*!
          * \brief Method to store the collected file, should be implemented by each subclass.
          */
          virtual void store(std::shared_ptr<te::da::DataSet> dataSet, const Collector & collector) = 0;


        };
      } // core    
    } // collector
  } // ws
} // terrama2

#endif // __TERRAMA2_WS_COLLECTOR_CORE_STORAGE_STRATEGY_HPP__

