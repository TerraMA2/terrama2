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

// TerraMA2
#include "../Exception.hpp"
#include "../Typedef.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/core/uri/URI.h>

// Qt
#include <QJsonObject>
#include <QObject>

namespace terrama2
{
  namespace core
  {
    class ProcessLogger : public QObject
    {
        Q_OBJECT

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
        enum MessageType
        {
          ERROR_MESSAGE     = 1,
          INFO_MESSAGE      = 2,
          WARNING_MESSAGE   = 3
        };


        struct MessageLog
        {
          uint32_t id =0;
          RegisterId log_id =0;
          MessageType type;
          std::string description = "";
          std::shared_ptr< te::dt::TimeInstantTZ > timestamp;
        };

        struct Log
        {
          RegisterId id = 0;
          ProcessId processId = 0;
          Status status;
          std::shared_ptr< te::dt::TimeInstantTZ > start_timestamp;
          std::shared_ptr< te::dt::TimeInstantTZ > data_timestamp;
          std::shared_ptr< te::dt::TimeInstantTZ > last_process_timestamp;
          std::string data = "";

          std::vector< MessageLog > messages;
        };

        /*!
         * \brief Class destructor
         */
        virtual ~ProcessLogger();

        /*!
         * \brief Log the start of the process.
         * \return The ID of table register
         */
        virtual RegisterId start(ProcessId processId) const;

        /*!
         * \brief Adds a log message to the process.
         * \param description Error description
         */
        virtual void log(MessageType messageType, const std::string &description, RegisterId registerId) const;


        /*!
         * \brief Log the end of process
         * \param dataTimestamp The las timestamp of data.
         */
        virtual void result(Status status, const std::shared_ptr<te::dt::TimeInstantTZ> &dataTimestamp,
                            RegisterId registerId) const;

        /*!
         * \brief Returns the process last log timestamp
         * \param processId The ID of the process
         * \return A TimeInstantTZ with the last time that process logged something
         */
        virtual std::shared_ptr< te::dt::TimeInstantTZ > getLastProcessTimestamp(const ProcessId processId) const;

        /*!
         * \brief Returns the last timestamp of a data
         * \param processId The ID of the process
         * \return A TimeInstantTZ with the data last timestamp
         */
        virtual std::shared_ptr< te::dt::TimeInstantTZ > getDataLastTimestamp(const ProcessId processId) const;

        /*!
         * \brief Get the logs of a process in a determined interval.
         *        The order of register is from last log to the first, and
         *        the first is 0.
         *        So if you want from 1º to 10º last logs, begin = 0 and end = 9,
         *        or from 3º to 5º last logs, begin = 2 and end = 4.
         * \param processId The ID of the process
         * \param begin The number order of the first wanted register
         * \param end The number in order of the last wanted register
         */
        std::vector<Log> getLogs(const ProcessId processId, uint32_t begin, uint32_t end) const;

        /*!
         * \brief Returns the process ID
         * \return Returns the process ID
         */
        virtual ProcessId processID(const RegisterId registerId) const;

      public slots:
        /*!
        * \brief Reset connection to log database information
        * \param connInfo Datasource connection information.
        */
        virtual void setConnectionInfo(const te::core::URI& uri) noexcept;

      protected:

        /*!
         * \brief Default constructor
         */
        ProcessLogger() = default;

        /*!
         * \brief Set the process logger data source
         */
        void setDataSource(te::da::DataSource* dataSource);

        /*!
         * \brief Store data in a Json to be logged after
         * \param tag A tag to identify data
         * \param value The data
         */
        void addValue(const std::string& tag, const std::string& value, const RegisterId registerId) const;


        /*!
         * \brief Store the name of the process log table and the message log table
         * \param tableName The log table name
         */
        void setTableName(std::string& tableName);

      private:
        /*!
         * \brief Log in the log table the data stored in Json
         */
        void updateData(const RegisterId registerId, const QJsonObject obj) const;

        std::string schema_ = "terrama2";
        std::string tableName_ = "";
        std::string messagesTableName_ = "";
        std::unique_ptr< te::da::DataSource > dataSource_;

        void closeConnection();
    };
  }
}
#endif // __TERRAMA2_CORE_PROCESSLOGGER_HPP__
