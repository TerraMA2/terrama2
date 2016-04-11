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
  \file terrama2/core/utility/ProcessLog.hpp

  \brief

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_CORE_PROCESSLOG_HPP__
#define __TERRAMA2_CORE_PROCESSLOG_HPP__

// TerraLib
#include <terralib/datatype/TimeInstantTZ.h>

// TerraMA2
#include "TimeUtils.hpp"

namespace terrama2
{
  namespace core
  {
    namespace ProcessLog
    {
      /*!
        \enum Status

        \brief Possible status of manipulate data.
      */
      enum Status
      {
        UNKNNOW     = 0, /*!< Is not possible to know de data status */
        ERROR       = 1, /*!< Was not possible to manipulate data */
        COLLECTING  = 2, /*!< Collecting Data */
        COLLECTED   = 3, /*!< Data was collected */
        NODATA      = 4, /*!< The source don't contains data */
        IMPORTED    = 4, /*!< The data was imported to TerraMA2 */
        PROCESSING  = 5, /*!< The data is now being processed */
        PROCESSED   = 6  /*!< Data processed */
      };

      void insert(uint64_t id, Status status, std::string originURI, std::string actualURI)
      {

      }

      void update(uint64_t id, Status status, std::string originURI, std::string actualURI, te::dt::TimeInstantTZ dataTimestamp)
      {

      }

      void error(uint64_t id, Status status, std::string description)
      {

      }

      std::shared_ptr< te::dt::TimeInstantTZ > getLastProcessDate(uint64_t id)
      {
        // VINICIUS:
        return terrama2::core::TimeUtils::now();
      }
    }
  }
}
#endif //__TERRAMA2_CORE_PROCESSLOG_HPP__
