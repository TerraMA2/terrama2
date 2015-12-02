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
  \file terrama2/collector/Log.cpp

  \brief Manage the log of data handled by collector service

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_COLLECTOR_LOG_HPP__
#define __TERRAMA2_COLLECTOR_LOG_HPP__

#include <iostream>

namespace terrama2
{
  namespace collector
  {
    class Log
    {
      public:

        enum Status
        {
          DOWNLOADED,
          IMPORTED,
          FAILED,
          UNKNOW
        };

        static uint64_t log(uint64_t dataSetItemId, std::string origin_uri, Status s);

        static void log(uint64_t dataSetItemId, std::vector< std::string > origin_uri, Status s);

        static void updateLog(uint64_t id, std::string uri, Status s, std::string data_timestamp);

        static void updateLog(std::string origin_uri, std::string uri, Status s, std::string data_timestamp);
    };
  }
}

#endif //__TERRAMA2_COLLECTOR_LOG_HPP__
