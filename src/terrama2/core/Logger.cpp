// TerraMA2
#include "Logger.hpp"

// Boost
#include <boost/log/trivial.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/expressions.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/support/date_time.hpp>


terrama2::core::Logger::Logger(const std::string filename)
  : log_(new boost::log::sources::logger)
{
  // temp
  boost::log::add_file_log(
        boost::log::keywords::file_name = filename,
        boost::log::keywords::open_mode = (std::ios_base::app | std::ios_base::out) & ~std::ios_base::in,
        boost::log::keywords::format = boost::log::expressions::stream
          << " [" << boost::log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d, %H:%M:%S") << "]"
          << ": <" << boost::log::expressions::attr<SeverityLevel>("Severity")
          << "> " << boost::log::expressions::message);

  boost::log::add_console_log(
      std::clog, boost::log::keywords::format = (boost::log::expressions::stream
                                                 << " ["
                                                 << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d, %H:%M:%S")
                                                 << "]"
                                                 << ": <"
                                                 << boost::log::expressions::attr<SeverityLevel>("Severity") << "> "
                                                 << boost::log::expressions::message));

  boost::log::add_common_attributes();

  BOOST_LOG_SEV(severityLevel_, WARNING) << "logger initialized warning";

  BOOST_LOG_SEV(severityLevel_, CRITICAL) << "logger initialized critical";

}

terrama2::core::Logger::~Logger()
{

}

void terrama2::core::Logger::write(const std::string& message, const SeverityLevel& level)
{
  BOOST_LOG_SEV(severityLevel_, level) << message.c_str();
}

//Logger& terrama2::core::Logger::operator<<(const std::string& message, const SeverityLevel& level)
//{
//  const char *messageLevel = nullptr;
//
//  switch (level) {
//    case NORMAL:
//      messageLevel = "Normal";
//      break;
//    case WARNING:
//      messageLevel = "Warning";
//      break;
//    case ERROR:
//      messageLevel = "Error";
//      break;
//    default:
//      messageLevel = "Critical";
//  }
//
//  BOOST_LOG_SEV(severityLevel_, level) << messageLevel << message;
//
//  return *this;
//}