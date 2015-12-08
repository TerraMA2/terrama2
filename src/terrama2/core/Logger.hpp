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

BOOST_LOG_INLINE_GLOBAL_LOGGER_CTOR_ARGS(my_logger, boost::log::sources::channel_logger_mt< >,
                                         (boost::log::keywords::channel = "general"))

#define TERRAMA2_LOG(logger, severity) BOOST_LOG_SEV(logger, severity) \
                                        << boost::log::add_value("RecordLine", __LINE__) \
                                        << boost::log::add_value("SrcFile", __FILE__) \
                                        << boost::log::add_value("CurrentFunction", BOOST_CURRENT_FUNCTION)

namespace terrama2
{
  // TerraMA2 Exception
  struct Exception;

  namespace core
  {

    class Logger : public te::common::Singleton<Logger>
    {

      friend class te::common::Singleton<Logger>;

      public:

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

          void operator() (const std::runtime_error& e) const;

          void operator() (const std::logic_error& e) const;
        };

        typedef boost::log::sinks::asynchronous_sink<boost::log::sinks::text_ostream_backend> text_sink;

        void initialize();
        void finalize();

        void debug(const char* message);
        void info(const char* message);
        void warning(const char* message);

        void addStream(boost::shared_ptr<std::ostream>& stream);
//        void setFormatter(const boost::log::formatter);

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
        boost::log::sources::logger_mt logger_;
        boost::shared_ptr<text_sink> sink_;
        boost::log::sources::severity_logger<SeverityLevel> level_;
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

      // Set up exception handler: all exceptions that occur while
      // logging through this logger, will be suppressed
      lg.set_exception_handler(boost::log::make_exception_suppressor());

      return lg;
    }
  } // end core


} // end terrama2

#endif // __TERRAMA2_CORE_LOGGER_HPP__
