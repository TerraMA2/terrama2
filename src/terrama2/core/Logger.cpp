// TerraMA2
#include "Logger.hpp"
#include "Exception.hpp"

// Boost
#include <boost/log/utility/exception_handler.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/syslog_backend.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>


// TODO: line number of file
void logFormatter(const boost::log::record_view& rec, boost::log::formatting_ostream& stream)
{
  const auto severity = boost::log::extract<terrama2::core::Logger::SeverityLevel>("Severity", rec);

  const auto timeStamp = boost::log::extract<boost::posix_time::ptime>("TimeStamp", rec);

  if (timeStamp.empty())
    return;

  boost::posix_time::time_facet* facet = new boost::posix_time::time_facet("%d-%b-%Y %H:%M:%S");
  stream.imbue(std::locale(stream.getloc(), facet));

  stream << "[" << timeStamp << "] ";

  if(severity.empty())
    return;

  switch (*severity)
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
    case terrama2::core::Logger::ERROR:
    {
      stream << "<ERROR>";
      stream << "{" << boost::log::extract<std::string>("SrcFile", rec) << ", " << boost::log::extract<int>("RecordLine", rec) << "}";
      break;
    }
    case terrama2::core::Logger::FATAL:
      stream << "<FATAL>";
      break;
    default:
      stream << "<" << *severity << ">";
  }

  stream << " " << rec[boost::log::expressions::smessage];

}

terrama2::core::Logger::Logger()
  : logger_(), sink_(new text_sink)
{
  boost::log::register_simple_formatter_factory<SeverityLevel , char>("Severity");
}

terrama2::core::Logger::~Logger()
{
}

void terrama2::core::Logger::initialize()
{
  sink_->locked_backend()->auto_flush(true);

  sink_->set_formatter(&logFormatter);

  // locale
//  std::locale loc = boost::locale::generator()("en_US.UTF-8");
//  sink_->imbue(loc);

  boost::log::core::get()->add_sink(sink_);

  // todo: exception handler
  boost::log::core::get()->set_exception_handler(boost::log::make_exception_handler<std::runtime_error, std::logic_error>(terrama2::core::Logger::ExceptionHandler()));
  boost::log::core::get()->add_global_attribute("TimeStamp", boost::log::attributes::local_clock());

//  BOOST_LOG_NAMED_SCOPE("TerraMA2");
}

void terrama2::core::Logger::finalize()
{
}

void terrama2::core::Logger::addStream(boost::shared_ptr<std::ostream>& stream)
{
  sink_->locked_backend()->add_stream(stream);
}

void terrama2::core::Logger::debug(const char* message)
{
  TERRAMA2_LOG_DEBUG() << message;
}

void terrama2::core::Logger::ExceptionHandler::operator()(const std::runtime_error &e) const
{
  std::cout << "runtime error ocorrido" << std::endl;
  terrama2::core::Logger::getInstance().warning("RUNTIME ERROR OCCURRED NOW");
}

void terrama2::core::Logger::ExceptionHandler::operator()(const std::logic_error& e) const
{
  std::cout << "logic error ocorrido" << std::endl;
  terrama2::core::Logger::getInstance().warning("LOGIC ERROR OCCURRED NOW");
  throw;
}

std::ostream& operator<< (std::ostream& strm, const terrama2::Exception& exception)
{
  const auto msg = boost::get_error_info<terrama2::ErrorDescription>(exception);
  std::string message = "** An exception occurred **! \t";
  if (msg != nullptr)
    message.append(msg->toStdString().c_str());

  return strm << message;
}

void terrama2::core::Logger::info(const char* message)
{
  TERRAMA2_LOG_DEBUG() << message;
}

void terrama2::core::Logger::warning(const char* message)
{
  TERRAMA2_LOG_WARNING() << message;
}

terrama2::core::Logger& terrama2::core::Logger::operator<<(const terrama2::Exception& e)
{
  const auto msg = boost::get_error_info<terrama2::ErrorDescription>(e);
  std::string message = "** An exception occurred **! \t";
  if (msg != nullptr)
    message.append(msg->toStdString().c_str());

//  TERRAMA2_LOG_ERROR() << message;

  return *this;
}

void terrama2::core::Logger::trace(const char *message)
{
  TERRAMA2_LOG_TRACE() << message;
}

void terrama2::core::Logger::fatal(const char *message)
{
  TERRAMA2_LOG_ERROR() << message;
}

//// todo: display the severity name in log instead enum number
//inline boost::log::formatting_ostream& operator<<(boost::log::formatting_ostream& stream,
//                                                  terrama2::Exception& level)
//{
//
//
//  return stream;
//}