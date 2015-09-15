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

enum Status
{
  ACTIVE,
  INACTIVE
};

enum DataProviderKind
{
  UNKNOWN_DATAPROVIDER_TYPE,
  PCD_TYPE,
  OCCURENCE_TYPE,
  GRID_TYPE
};

class DataProvider
{
  uint64_t                  id_;
  std::string               name_;
  std::string               description_;
  enum DataProviderKind     kind_;
  std::string               uri_;
  enum Status               status_;
};

struct findDataProviderResponse { DataProvider return_;};

//struct listDataProviderResponse { std::vector<DataProvider> return_ };

enum DataSetKind
{
  UNKNOWN_DATASET_TYPE,
  FTP_TYPE,
  HTTP_TYPE,
  FILE_TYPE,
  WFS_TYPE,
  WCS_TYPE
};

class DataSet
{
  uint64_t                  id_;
  std::string               name_;
  std::string               description_;
  enum Status               status_;
//  DataProviderPtr           dataProvider_;
  enum DataSetKind          kind_;  
//  te::dt::TimeDuration      dataFrequency_;
//  te::dt::TimeDuration      schedule_;
//  te::dt::TimeDuration      scheduleRetry_;
//  te::dt::TimeDuration      scheduleTimeout_;
};

struct findDataSetResponse {DataSet return_;};
