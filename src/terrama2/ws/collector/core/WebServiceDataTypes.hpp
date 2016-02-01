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
  uint64_t      id;
  std::string   name;
  std::string   description;
  uint32_t      kind;
  uint32_t      origin;
  std::string   uri;
  uint32_t      status;
};

struct Intersection
{
  uint64_t datasetId;
  std::vector<std::string > attributeMap_keys;
  std::vector<std::vector<std::string> > attributeMap_values;
  std::vector<uint64_t> bandMap_keys;
  std::vector<std::string> bandMap_values;
};


struct DataSetItem
{
  uint32_t      kind;
  uint64_t      id;
  uint64_t      dataset;
  uint32_t      status;
  std::string   mask;
  std::string   timezone;
  std::string   path;
  uint64_t      srid;

// the filters values need to be initialized to avoid to create non-existent filters
  uint64_t          filter_datasetItem;
  std::string       filter_discardBefore_date;
  std::string       filter_discardBefore_time;
  std::string       filter_discardBefore_timezone;
  std::string       filter_discardAfter_date;
  std::string       filter_discardAfter_time;
  std::string       filter_discardAfter_timezone;
  std::string       filter_geometry;
  uint64_t          filter_geometry_srid;
  double            filter_value;
  uint32_t          filter_expressionType;
  std::string       filter_bandFilter;

  std::vector< std::string > metadata_keys;
  std::vector< std::string > metadata_values;
};


struct DataSet
{
  uint64_t                          id;
  std::string                       name;
  std::string                       description;
  uint32_t                          status;
  uint64_t                          data_provider_id;
  uint32_t                          kind;
  std::string                       data_frequency;
  std::string                       schedule;
  std::string                       schedule_retry;
  std::string                       schedule_timeout;
  std::vector< struct DataSetItem > dataset_items;
  struct Intersection               intersection;

  std::vector< std::string > metadata_keys;
  std::vector< std::string > metadata_values;

};
