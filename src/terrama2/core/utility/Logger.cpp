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
  case terrama2::core::Logger::ERROR:
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
{
}

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

class LogSyncronizer : public std::streambuf
{
public:
  LogSyncronizer(std::streambuf* sb1, terrama2::core::TcpManager* tcpManager = nullptr)
   : sb1(sb1), sb2(new std::stringbuf()), tcpManager(tcpManager)
  { }
  ~LogSyncronizer() { delete sb2; }

private:
  virtual int sync()
  {
    int const r1 = sb1->pubsync();
    int const r2 = sb2->pubsync();
    if(tcpManager)
    {
      std::string str = sb2->str();
      bool sent = tcpManager->sendLog(str);
      return r1 == 0 && r2 == 0 && sent ? 0 : -1;
    }
    else
      return r1 == 0 && r2 == 0 ? 0 : -1;
  }

  virtual int_type overflow(int_type c)
  {
    if(c == EOF)
    {
      return !EOF;
    }
    else
    {
      int const r1 = sb1->sputc(c);
      int const r2 = sb2->sputc(c);
      return r1 == EOF || r2 == EOF ? EOF : c;
    }
  }

private:
  std::streambuf* sb1;
  std::stringbuf* sb2;
  terrama2::core::TcpManager* tcpManager;
};

void terrama2::core::Logger::addStream(const std::string& stream_name, terrama2::core::TcpManager* tcpManager)
{
  loggerPath_ = stream_name;

  boost::shared_ptr<std::ostream> stream_out(&std::clog, boost::null_deleter());
  boost::shared_ptr<std::ostream> stream_file(new std::ofstream(stream_name, std::ostream::app));

  // sync every log sent to file with the tcp manager
  LogSyncronizer sync(stream_file->rdbuf(), tcpManager);

  sink_->locked_backend()->add_stream(stream_file);
  sink_->locked_backend()->add_stream(stream_out);
}
