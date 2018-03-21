// TerraMA2
#include "Logger.hpp"
#include "../network/TcpManager.hpp"

// Boost
#include <boost/log/attributes.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/core/null_deleter.hpp>

// STL
#include <fstream>

void logFormatter(const boost::log::record_view& rec, boost::log::formatting_ostream& stream)
{
  const auto severity = boost::log::extract<terrama2::core::Logger::SeverityLevel>("Severity", rec);

  const auto timeStamp = boost::log::extract<boost::posix_time::ptime>("TimeStamp", rec);

  if(timeStamp.empty())
    return;

  boost::posix_time::time_facet* facet = new boost::posix_time::time_facet("%d-%b-%Y %H:%M:%S");
  stream.imbue(std::locale(stream.getloc(), facet));

  stream << "[" << timeStamp << "] ";

  if(severity.empty())
    return;

  switch(*severity)
  {
  case terrama2::core::Logger::TRACE:
    stream << "<TRACE>";
    break;
  case terrama2::core::Logger::DEBUG:
    stream << "<DEBUG>";
    break;
  case terrama2::core::Logger::INFO:
    stream << "<INFO>";
    break;
  case terrama2::core::Logger::WARNING:
    stream << "<WARNING>";
    break;
  case terrama2::core::Logger::T_ERROR:
  {
    stream << "<ERROR>";
    stream << " {" << boost::log::extract<std::string>("SrcFile", rec) << ", " << boost::log::extract<int>("RecordLine", rec) << "}";
    break;
  }
  case terrama2::core::Logger::FATAL:
    stream << "<FATAL>";
    stream << " {" << boost::log::extract<std::string>("SrcFile", rec) << ", " << boost::log::extract<int>("RecordLine", rec) << "}";
    break;
  default:
    stream << "<" << *severity << ">";
  }

  stream << " " << rec[boost::log::expressions::smessage];
}

terrama2::core::Logger::Logger() : sink_(new text_sink)
{
}

terrama2::core::Logger::~Logger()
= default;

void terrama2::core::Logger::initialize()
{
  sink_->locked_backend()->auto_flush(true);

  sink_->set_formatter(&logFormatter);

  boost::log::core::get()->add_sink(sink_);

  boost::log::core::get()->add_global_attribute("TimeStamp", boost::log::attributes::local_clock());

  enableLog();
}

void terrama2::core::Logger::enableLog()
{
  boost::log::core::get()->set_logging_enabled(true);
}

void terrama2::core::Logger::disableLog()
{
  boost::log::core::get()->set_logging_enabled(false);
}

const std::string& terrama2::core::Logger::path() const
{
  return loggerPath_;
}

void terrama2::core::Logger::addStream(const std::string& stream_name)
{
  loggerPath_ = stream_name;

  boost::shared_ptr<std::ostream> stream_out(&std::clog, boost::null_deleter());
  boost::shared_ptr<std::ostream> stream_file(new std::ofstream(stream_name, std::ostream::app));

  sink_->locked_backend()->add_stream(stream_file);
  sink_->locked_backend()->add_stream(stream_out);
}
