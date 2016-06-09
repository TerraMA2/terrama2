#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/ProcessLogger.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>

class Logger : public terrama2::core::ProcessLogger
{
public:

  /*!
   * \brief Class constructor
   */
  Logger(std::map<std::string, std::string> connInfo)
    : ProcessLogger(connInfo)
  {
    setTableName("example_processlogger_1");
  }

  /*!
   * \brief Class destructor
   */
  virtual ~Logger() = default;

  /*!
   * \brief The method addValue is protected in ProcessLog, so was needed to implement a method
   * that calls it.
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
    std::map<std::string, std::string> connInfo { {"PG_HOST", "localhost"},
                                                  {"PG_PORT", "5432"},
                                                  {"PG_USER", "postgres"},
                                                  {"PG_PASSWORD", "postgres"},
                                                  {"PG_DB_NAME", "example"},
                                                  {"PG_CONNECT_TIMEOUT", "4"},
                                                  {"PG_CLIENT_ENCODING", "UTF-8"}
                                                };

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
    std::vector< Logger::Log > logs = log.getLogs(process_id, 1 , 10);

    logs.size();

  }
  catch(...)
  {

  }

  terrama2::core::finalizeTerraMA();
}
