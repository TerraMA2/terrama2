#ifndef __TERRAMA2_CORE_LOGGER_HPP__
#define __TERRAMA2_CORE_LOGGER_HPP__

// TerraMA2
#include "../Config.hpp"
#include "../Exception.hpp"

// Terralib
#include <terralib/common/Singleton.h>

// Boost
#include <boost/shared_ptr.hpp>
#include <boost/log/common.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>


namespace terrama2
{
  namespace core
  {
    /*!
      \brief Logs information for TerraMA², should not be used directly.

      The Logger class is a singleton used to store log information, logging should be made using the macros:
       - TERRAMA2_LOG_TRACE()
       - TERRAMA2_LOG_DEBUG()
       - TERRAMA2_LOG_INFO()
       - TERRAMA2_LOG_WARNING()
       - TERRAMA2_LOG_ERROR()
       - TERRAMA2_LOG_FATAL()


    */
    class TMCOREEXPORT Logger : public te::common::Singleton<Logger>
    {

      friend class te::common::Singleton<Logger>;

      public:

        //! Defines level of severity message in \c terrama2 log.
        enum SeverityLevel
        {
          TRACE,   ///< Define a trace system alert in file. It will be useful to trace where it has gone
          DEBUG,   ///< Define a debug severity message in file. It will be useful in development side.
          INFO,    ///< Define a info message for notify a common action.
          WARNING, ///< Define a warning alert in log file, representing that something may go wrong.
          ERROR,   ///< Define a error system alert. Useful when something unexpected occurs.
          FATAL    ///< Define a fatal application alert. A runtime error or uncaught exception that shutdown application.
        };

        /*!
          \brief An abreviation of a boost text log stream
        */
        typedef boost::log::sinks::asynchronous_sink<boost::log::sinks::text_ostream_backend> text_sink;

        //! It initializes configuration to global boost logger
        void initialize();

        //! It enables the global TerraMA2 logger to allow display status message.
        void enableLog();

        //! It disables the global TerraMA2 logger to avoid display status message.
        void disableLog();

        //! It retrieves the stream name where the terrama2.log will be saved
        const std::string& path() const;

        //! It sets path of terrama2 log and add ostream to sink backend
        void addStream(const std::string& stream_name);

      protected:
        /*!
          \brief Protected default constructor.
        */
        Logger();

        //! Destructor
        ~Logger();

      private:
        boost::shared_ptr<text_sink> sink_; //!< Sink for handling records.
        std::string loggerPath_;            //!< Path to the log.
    }; // end class logger

    //! Override operator<< to enable sets terrama2 exception in log. It formats the exception and put it in stream
    inline std::ostream& operator<<(std::ostream& stream, const terrama2::Exception& exception)
    {
      const auto msg = boost::get_error_info<terrama2::ErrorDescription>(exception);
      std::string message = "** An exception occurred **! \t";
      if (msg != nullptr)
        message.append(msg->toStdString().c_str());
      return stream << message;
    }

  } // end core
} // end terrama2

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(terrama2_logger, boost::log::sources::severity_logger<terrama2::core::Logger::SeverityLevel>)

#define TERRAMA2_LOG(severity) BOOST_LOG_SEV(terrama2_logger::get(), severity) \
                                                      << boost::log::add_value("RecordLine", __LINE__) \
                                                      << boost::log::add_value("SrcFile", __FILE__) \
                                                      << boost::log::add_value("CurrentFunction", BOOST_CURRENT_FUNCTION)

#define TERRAMA2_LOG_TRACE() TERRAMA2_LOG(terrama2::core::Logger::TRACE)
#define TERRAMA2_LOG_DEBUG() TERRAMA2_LOG(terrama2::core::Logger::DEBUG)
#define TERRAMA2_LOG_INFO() TERRAMA2_LOG(terrama2::core::Logger::INFO)
#define TERRAMA2_LOG_WARNING() TERRAMA2_LOG(terrama2::core::Logger::WARNING)
#define TERRAMA2_LOG_ERROR() TERRAMA2_LOG(terrama2::core::Logger::ERROR)
#define TERRAMA2_LOG_FATAL() TERRAMA2_LOG(terrama2::core::Logger::FATAL)

//! Override operator<< to enable sets QString in log.
inline std::ostream& operator<<(std::ostream& stream, const QString message)
{
  return stream << message.toStdString();
}


#endif // __TERRAMA2_CORE_LOGGER_HPP__
