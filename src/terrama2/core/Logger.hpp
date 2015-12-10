#ifndef __TERRAMA2_CORE_LOGGER_HPP__
#define __TERRAMA2_CORE_LOGGER_HPP__

// terralib
#include <terralib/common/Singleton.h>

// STL
#include <ostream>
#include <fstream>

// Boost
#include <boost/shared_ptr.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/exception_handler.hpp>
#include "Exception.hpp"


namespace terrama2
{
  // TerraMA2 Exception
  struct Exception;

  namespace core
  {

    struct DataAccessError;

    class Logger : public te::common::Singleton<Logger>
    {

      friend class te::common::Singleton<Logger>;

      public:

        struct SeverityTag;

        enum SeverityLevel
        {
          TRACE,
          DEBUG,
          INFO,
          WARNING,
          ERROR,
          FATAL
        };

        // TODO: make the exception handler to work
        struct ExceptionHandler
        {
          typedef void result_type;

          result_type operator() (const std::runtime_error& e) const;

          result_type operator() (const std::logic_error& e) const;
        };

        typedef boost::log::sinks::asynchronous_sink<boost::log::sinks::text_ostream_backend> text_sink;

        void initialize();
        void finalize();

        void debug(const char* message);
        void info(const char* message);
        void warning(const char* message);
        void trace(const char* message);
        void fatal(const char* message);

        void addStream(boost::shared_ptr<std::ostream>& stream);

        Logger& operator<<(const terrama2::Exception& e);

//        template<typename T>
//        Logger& operator<<(const T& value)
//        {
//          TERRAMA2_LOG(logger_, DEBUG) << value;
////          BOOST_LOG(my_logger::get()) << value;
//          return *this;
//        }

      protected:
        Logger();
        ~Logger();

      private:
        boost::log::sources::severity_logger<SeverityLevel> logger_;
        boost::shared_ptr<text_sink> sink_;
    };

// TODO: make the exception handler to work
    class LoggerMT :
        public boost::log::sources::basic_composite_logger<char, LoggerMT,
                                                           boost::log::sources::multi_thread_model<boost::shared_mutex>,
                                                           boost::log::sources::features<boost::log::sources::severity<
                                                               terrama2::core::Logger::SeverityLevel>,
                                                               boost::log::sources::exception_handler>>
    {
      BOOST_LOG_FORWARD_LOGGER_MEMBERS(LoggerMT)
    };

    BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(my_logger, LoggerMT)
    {
      terrama2::core::LoggerMT lg;

      lg.set_exception_handler(boost::log::make_exception_suppressor());

      return lg;
    }
  } // end core

} // end terrama2

BOOST_LOG_INLINE_GLOBAL_LOGGER_CTOR_ARGS(terrama2_logger, boost::log::sources::severity_logger<terrama2::core::Logger::SeverityLevel>,
                                        (boost::log::keywords::channel = "general"))

#define TERRAMA2_LOG(severity) BOOST_LOG_SEV(terrama2_logger::get(), severity) \
                                                      << boost::log::add_value("RecordLine", __LINE__) \
                                                      << boost::log::add_value("SrcFile", __FILE__) \
                                                      << boost::log::add_value("CurrentFunction", BOOST_CURRENT_FUNCTION)

#define TERRAMA2_LOG_TRACE() TERRAMA2_LOG(terrama2::core::Logger::TRACE)
#define TERRAMA2_LOG_DEBUG() TERRAMA2_LOG(terrama2::core::Logger::DEBUG)
#define TERRAMA2_LOG_WARNING() TERRAMA2_LOG(terrama2::core::Logger::WARNING)
#define TERRAMA2_LOG_ERROR() TERRAMA2_LOG(terrama2::core::Logger::ERROR)

inline boost::log::formatting_ostream&
operator<< (boost::log::formatting_ostream& strm, const terrama2::core::DataAccessError& value)
{
//  strm.stream() << value;
  return strm;
}

#endif // __TERRAMA2_CORE_LOGGER_HPP__
