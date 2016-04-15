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
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/datatype/TimeInstantTZ.h>

// Qt
#include <QJsonObject>

namespace terrama2
{
  namespace core
  {
    /*!
      \enum Status

      \brief Possible status of manipulate data.
    */
    enum Status
    {
      UNKNOWN     = 0, /*!< Is not possible to know de data status */
      ERROR       = 1, /*!< Error during process */
      START       = 2, /*!< The process started */
      DOWNLOADED  = 3, /*!< The data was downloaded */
      DONE        = 4  /*!< Process finished */
    };

    class ProcessLog
    {
      public:
        ProcessLog(uint64_t processID, std::map < std::string, std::string > connInfo);

        void start(uint64_t processID);

        void addValue(std::string tag, std::string value);

        void updateData();

        void error(std::string description);

        void done(te::dt::TimeInstantTZ dataTimestamp);

        std::shared_ptr< te::dt::TimeInstantTZ > getLastProcessDate();

        void setTableName(std::string tableName);

        uint64_t primaryKey();

        uint64_t processID();

      private:
        uint64_t processID_;
        uint64_t primaryKey_;
        std::string tableName_;
        QJsonObject obj_;
        std::shared_ptr< te::da::DataSource > dataSource_;

    };
  }
}
#endif //__TERRAMA2_CORE_PROCESSLOG_HPP__
