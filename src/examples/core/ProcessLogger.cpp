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
  \file examples/core/ProcessLogger.cpp

  \brief Example about how to log your process using TerraMA2
          Process Logger base class

  \author Vinicius Campanha
*/


// TerraMA2
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/ProcessLogger.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>

// STL
#include <vector>
#include <iostream>


/* Create a derived class is required to access protected members
*   from the base class Process Logger
*/
class Logger : public terrama2::core::ProcessLogger
{
public:

  /*!
   * \brief Class constructor
   */
  Logger(std::map<std::string, std::string> connInfo)
    : ProcessLogger(connInfo)
  {
    // Need to set the wanted log table name
    setTableName("example_processlogger_1");
  }

  /*!
   * \brief Class destructor
   */
  virtual ~Logger() = default;

  /*!
   * \brief The method addValue is protected in ProcessLog, so is needed implement a method
   * to calls it.
   */
  void logValue(const std::string tag, const std::string value, const RegisterId registerId) const
  {
    addValue(tag, value, registerId);
  }
};


int main(int argc, char* argv[])
{
  terrama2::core::initializeTerraMA();

  try
  {

    std::map<std::string, std::string> connInfo { {"PG_HOST", TERRAMA2_DATABASE_HOST},
                                                  {"PG_PORT", TERRAMA2_DATABASE_PORT},
                                                  {"PG_USER", TERRAMA2_DATABASE_USERNAME},
                                                  {"PG_PASSWORD", TERRAMA2_DATABASE_PASSWORD},
                                                  {"PG_DB_NAME", TERRAMA2_DATABASE_DBNAME},
                                                  {"PG_CONNECT_TIMEOUT", "4"},
                                                  {"PG_CLIENT_ENCODING", "UTF-8"}
                                                };

    // Use the derived class to log
    Logger log(connInfo);

    ProcessId processId = 1;

    // Start logger for a Process
    RegisterId registerID = log.start(processId);

    // Log informations with tags descriptions
    log.logValue("tag1", "value1", registerID);
    log.logValue("tag2", "value2", registerID);
    log.logValue("tag1", "value3", registerID);
    log.logValue("tag2", "value4", registerID);

    // Log errors
    log.error("Unit Test Error", registerID);
    log.error("Unit Test second Error", registerID);

    // Log informations
    log.info("Unit Test Info", registerID);
    log.info("Unit Test seconde Info", registerID);

    // Log the end of process with the timestamp of processed data
    std::shared_ptr< te::dt::TimeInstantTZ > data_dateTime = terrama2::core::TimeUtils::nowUTC();
    log.done(data_dateTime, registerID);

    // Get the process ID consulting by register ID
    ProcessId process_id = log.processID(registerID);

    // Get the last timestamp this process was executed
    std::shared_ptr< te::dt::TimeInstantTZ > lastProcessTimestamp = log.getLastProcessTimestamp(process_id);

    // Get the last data timestamp that this data manipulated
    std::shared_ptr< te::dt::TimeInstantTZ > lastDataTimestamp = log.getDataLastTimestamp(process_id);

    // Get the first to tenth logs from this process
    std::vector< Logger::Log > logs = log.getLogs(process_id, 0 , 9);

/*
    Logger::Log log1= logs.at(0);

    std::cout << log1.id << std::endl;
    std::cout << int(log1.processId) << std::endl;
    std::cout << int(log1.status) << std::endl;
    std::cout << log1.start_timestamp->toString() << std::endl;
    std::cout << log1.data_timestamp->toString() << std::endl;
    std::cout << log1.last_process_timestamp->toString() << std::endl;
    std::cout << log1.data << std::endl;

    Logger::MessageLog mLog = log1.messages.at(0);

    std::cout << mLog.id << std::endl;
    std::cout << int(mLog.log_id) << std::endl;
    std::cout << int(mLog.type) << std::endl;
    std::cout << mLog.description << std::endl;
    std::cout << mLog.timestamp->toString() << std::endl;
*/
  }
  catch(...)
  {
    std::cout << "Error in Process Logger example!" << std::endl;
  }

  terrama2::core::finalizeTerraMA();
}
