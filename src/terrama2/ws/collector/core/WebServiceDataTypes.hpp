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
  \file terrama2/ws/collector/core/WebServiceDataTypes.hpp

  \brief TerraMA2 Collector gSOAP data types.

  \author Vinicius Campanha
 */

//#import "stlvector.h"

struct DataProvider
{
  uint64_t      id;
  std::string   name;
  std::string   description;
  uint32_t      kind;
  std::string   uri;
  uint32_t      status;
};

struct DataProviderResponse { DataProvider return_; };

//struct listDataProviderResponse { std::vector<DataProvider> return_ };

struct DataSet
{
  uint64_t      id;
  std::string   name;
  std::string   description;
  uint32_t      status;
  uin64_t       data_provider_id;
  uint32_t      kind;
  uint32_t      data_frequency;
  std::string   schedule;
  uint32_t      schedule_retry;
  uint32_t      schedule_timeout;
};

struct DataSetResponse { DataSet return_; };
