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


struct DataProvider
{
  uint64_t      id = 0;
  std::string   name = "";
  std::string   description = "";
  uint32_t      kind = 0;
  std::string   uri = "";
  uint32_t      status = 0;
};


struct DataSetItem
{
  uint32_t      kind = 0;
  uint64_t      id = 0;
  uint64_t      dataset = 0;
  uint32_t      status = 0;
  std::string   mask = "";
  std::string   timezone = "";

  uint64_t          filter_datasetItem = 0;
  std::string       filter_discardBefore = "";
  std::string       filter_discardAfter = "";
  std::string       filter_geometry = "";
  double            filter_value;
  uint32_t          filter_expressionType = 0;
  std::string       filter_bandFilter = "";

  std::vector< std::string > storageMetadata_keys;
  std::vector< std::string > storageMetadata_values;
};


struct DataSet
{
  uint64_t                          id = 0;
  std::string                       name = "";
  std::string                       description = "";
  uint32_t                          status = 0;
  uint64_t                          data_provider_id = 0;
  uint32_t                          kind = 0;
  std::string                       data_frequency = "";
  std::string                       schedule = "";
  std::string                       schedule_retry = "";
  std::string                       schedule_timeout = "";
  std::vector< struct DataSetItem > dataset_items;
};
