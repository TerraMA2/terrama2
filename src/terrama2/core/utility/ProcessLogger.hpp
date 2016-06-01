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
  \file terrama2/core/utility/ProcessLogger.hpp

  \brief

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_CORE_PROCESSLOGGER_HPP__
#define __TERRAMA2_CORE_PROCESSLOGGER_HPP__

#include "../Typedef.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/datatype/TimeInstantTZ.h>

// Qt
#include <QJsonObject>

namespace terrama2
{
  namespace core
  {
    class ProcessLogger
    {
    public:

      /*!
        \enum Status

        \brief Possible status of manipulate data.
      */
      enum Status
      {
        ERROR       = 1, /*!< Error during process */
        START       = 2, /*!< The process started */
        DOWNLOADED  = 3, /*!< The data was downloaded */
        DONE        = 4  /*!< Process finished */
      };

      /*!
        \enum messageType

        \brief Possible status of logged messages.
      */
      enum messageType
      {
        ERROR_MESSAGE     = 1,
        INFO_MESSAGE      = 2,
        WARNING_MESSAGE   = 3
      };

      /*!
       * \brief Class Constructor.
       * \param processID ID of the process to log.
       * \param connInfo Datasource connection information.
       */
      ProcessLogger(const std::map < std::string, std::string > connInfo);

      /*!
       * \brief Class destructor
       */
      ~ProcessLogger();

      /*!
       * \brief Log the start of the process.
       * \return The ID of table register
       */
      RegisterId start(ProcessId processId) const;

      /*!
       * \brief Store data in a Json to be logged after
       * \param tag A tag to identify data
       * \param value The data
       */
      void addValue(const std::string tag, const std::string value, const RegisterId registerId) const;

      /*!
       * \brief Log an error in the process
       * \param description Error description
       */
      void error(const std::string description, const RegisterId registerId) const;

      /*!
       * \brief Log the end of process
       * \param dataTimestamp The las timestamp of data.
       */
      void done(const std::shared_ptr< te::dt::TimeInstantTZ > dataTimestamp, const RegisterId registerId) const;

      /*!
       * \brief Returns the process last log timestamp
       * \return A TimeInstantTZ with the last time that process logged something
       */
      std::shared_ptr< te::dt::TimeInstantTZ > getLastProcessTimestamp(const ProcessId processId) const;

      /*!
       * \brief Returns the last timestamp of a data
       * \return A TimeInstantTZ with the data last timestamp
       */
      std::shared_ptr< te::dt::TimeInstantTZ > getDataLastTimestamp(const RegisterId registerId) const;

      /*!
       * \brief Returns the process ID
       * \return Returns the process ID
       */
      ProcessId processID(const RegisterId registerId) const;

    protected:
      /*!
       * \brief Store the table name of the process log
       * \param tableName The log table name
       */
      void setTableName(const std::string tableName);
      std::string getMessagesTableName(const RegisterId registerId) const;


    private:
      /*!
       * \brief Log in the log table the data stored in Json
       */
      void updateData(const RegisterId registerId, const QJsonObject obj) const;


    private:
      std::string tableName_ = "";
      std::string messagesTableName_ = "";
      std::shared_ptr< te::da::DataSource > dataSource_;

    };
  }
}
#endif // __TERRAMA2_CORE_PROCESSLOGGER_HPP__
