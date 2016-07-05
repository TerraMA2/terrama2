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
  \file terrama2/core/DataSetMapper.hpp

  \brief

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_DATASET_MAPPER_HPP__
#define __TERRAMA2_CORE_DATASET_MAPPER_HPP__

#include <terralib/dataaccess/dataset/DataSet.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace terrama2
{
  namespace core
  {
    class DataSetMapper
    {
      public:
        DataSetMapper(std::shared_ptr<te::da::DataSet> dataSet, std::string pkProperty);

        ~DataSetMapper() = default;
        DataSetMapper(const DataSetMapper& other) = delete;
        DataSetMapper(DataSetMapper&& other) = delete;
        DataSetMapper& operator=(const DataSetMapper& other) = delete;
        DataSetMapper& operator=(DataSetMapper&& other) = delete;

        std::unique_ptr< te::dt::AbstractData > getValue(const std::string& key, const std::string& attribute) const;

      private:
        std::unordered_map<std::string, size_t> generateStringPkToLineMap(std::shared_ptr<te::da::DataSet> dataSet, std::string pkProperty) const;

        std::shared_ptr<te::da::DataSet> dataSet_;
        std::unordered_map<std::string, size_t> lineMap_;
    };
  } /* core */
} /* terrama2 */

#endif // __TERRAMA2_CORE_DATASET_MAPPER_HPP__
