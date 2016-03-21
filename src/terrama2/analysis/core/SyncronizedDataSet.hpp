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
  \file terrama2/analysis/core/SyncronizedDataSet.hpp

  \brief Thread-safe dataset.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_ANALYSIS_CORE_SyncronizedDataSet_HPP__
#define __TERRAMA2_ANALYSIS_CORE_SyncronizedDataSet_HPP__

#include <terralib/dataaccess/dataset/DataSet.h>

#include <memory>
#include <mutex>

namespace terrama2
{
  namespace analysis
  {
    namespace core
    {
      class SyncronizedDataSet
      {
        public:
          SyncronizedDataSet(std::shared_ptr<te::da::DataSet> dataset);
          ~SyncronizedDataSet();

          std::shared_ptr<te::gm::Geometry> getGeometry(uint64_t row, uint64_t columnIndex) const;
          std::string getString(uint64_t row, uint64_t columnIndex) const;
          double getDouble(uint64_t row, uint64_t columnIndex) const;
          bool getBool(uint64_t row, uint64_t columnIndex) const;
          int16_t getInt16(uint64_t row, uint64_t columnIndex) const;
          int32_t getInt32(uint64_t row, uint64_t columnIndex) const;
          int64_t getInt64(uint64_t row, uint64_t columnIndex) const;
          std::shared_ptr<te::rst::Raster> getRaster(uint64_t row, uint64_t columnIndex) const;
          std::auto_ptr<te::dt::DateTime> getDateTime(uint64_t row, uint64_t columnIndex) const;
          bool isNull(uint64_t row, std::size_t columnIndex) const;


          std::shared_ptr<te::gm::Geometry> getGeometry(uint64_t row, std::string columnName) const;
          std::string getString(uint64_t row, std::string columnName) const;
          double getDouble(uint64_t row, std::string columnName) const;
          bool getBool(uint64_t row, std::string columnName) const;
          int16_t getInt16(uint64_t row, std::string columnName) const;
          int32_t getInt32(uint64_t row, std::string columnName) const;
          int64_t getInt64(uint64_t row, std::string columnName) const;
          std::shared_ptr<te::rst::Raster> getRaster(uint64_t row, std::string columnName) const;
          std::auto_ptr<te::dt::DateTime> getDateTime(uint64_t row, std::string columnName) const;
          bool isNull(uint64_t row, std::string columnName) const;

          uint64_t size() const;

          std::shared_ptr<te::da::DataSet> dataset() const;

        protected:
          std::shared_ptr<te::da::DataSet> dataset_;
          mutable std::mutex mutex_;

      };
    }
  }
}

#endif //__TERRAMA2_ANALYSIS_CORE_SyncronizedDataSet_HPP__
