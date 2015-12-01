#ifndef __TERRAMA2_CORE_LOGGER_HPP__
#define __TERRAMA2_CORE_LOGGER_HPP__

// STL
#include <memory>
#include <iostream>

// check: constant values
#define BOOST_LOG_DYN_LINK 1

#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>


namespace terrama2
{
  namespace core
  {
    class Logger
    {
      public:
        enum SeverityLevel
        {
          NORMAL,
          WARNING,
          ERROR,
          CRITICAL
        };

        Logger(const std::string filename);
        ~Logger();

        void write(const std::string& message, const SeverityLevel& level = NORMAL);

//        Logger& operator<<(const std::string& message, const SeverityLevel& level = NORMAL);

      private:
        std::shared_ptr<boost::log::sources::logger> log_;
        boost::log::sources::severity_logger<SeverityLevel> severityLevel_;
    };
  }
}

#endif // __TERRAMA2_CORE_LOGGER_HPP__
