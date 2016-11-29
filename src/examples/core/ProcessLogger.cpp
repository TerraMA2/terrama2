
// TerraMA2
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>
#include <terrama2/core/utility/ProcessLogger.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/Config.hpp>

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
  Logger()
    : ProcessLogger()
  {

  }

  /*!
   * \brief Class destructor
   */
  virtual ~Logger() = default;

  /*!
   * \brief The method start is protected in ProcessLog, so is needed implement a method
   * to calls it.
   */
  uint32_t startLog(uint32_t processId)
  {
    // Need to set the wanted log table name
    std::string tableName = "example_processlogger_6";
    setTableName(tableName);
    return start(processId);
  }

  /*!
   * \brief The method addValue is protected in ProcessLog, so is needed implement a method
   * to calls it.
   */
  void logValue(const std::string& tag, const std::string& value, const RegisterId registerId) const
  {
    addValue(tag, value, registerId);
  }
};


int main(int argc, char* argv[])
{
  terrama2::core::TerraMA2Init terramaRaii("example", 0);

  try
  {

    te::core::URI uri("pgsql://"+TERRAMA2_DATABASE_USERNAME+":"+TERRAMA2_DATABASE_PASSWORD+"@"+TERRAMA2_DATABASE_HOST+":"+TERRAMA2_DATABASE_PORT+"/"+TERRAMA2_DATABASE_DBNAME);

 // Use the derived class to log
    Logger log;
    log.setConnectionInfo(uri);

    ProcessId processId = 1;

    // Start logger for a Process
    RegisterId registerID = log.startLog(processId);

    // Log informations with tags descriptions
    log.logValue("tag1", "value1", registerID);
    log.logValue("tag2", "value2", registerID);
    log.logValue("tag1", "value3", registerID);
    log.logValue("tag2", "value4", registerID);

    // Log errors
    log.log(Logger::ERROR_MESSAGE, "Unit Test Error", registerID);
    log.log(Logger::ERROR_MESSAGE, "Unit Test second Error", registerID);

    // Log informations
    log.log(Logger::INFO_MESSAGE, "Unit Test Info", registerID);
    log.log(Logger::INFO_MESSAGE, "Unit Test seconde Info", registerID);

    // Log the end of process with the timestamp of processed data
    std::shared_ptr< te::dt::TimeInstantTZ > data_dateTime = terrama2::core::TimeUtils::nowUTC();
    log.result(Logger::DONE, data_dateTime, registerID);

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
  catch(const terrama2::Exception& e)
  {
    std::cout << "Error in Process Logger example: " << boost::get_error_info<terrama2::ErrorDescription>(e) << std::endl;
  }
  catch(boost::exception& e)
  {
    std::cout << "Error in Process Logger example: " << boost::diagnostic_information(e) << std::endl;
  }
  catch(std::exception& e)
  {
    std::cout << "Error in Process Logger example: " << e.what() << std::endl;
  }
  catch(...)
  {
    std::cout << "Error in Process Logger example!" << std::endl;
  }


}
