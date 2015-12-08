#include "TsLogger.hpp"

#include <terrama2/core/Logger.hpp>
#include <terrama2/core/Exception.hpp>


void TsLogger::init()
{

}

void TsLogger::testLoad()
{
  boost::shared_ptr<std::ostream> stream_out(&std::clog, boost::null_deleter());
  boost::shared_ptr<std::ostream> stream_file(new std::ofstream("/home/raphael/Documents/tmp/terrama2.log", std::ostream::app));

  terrama2::core::Logger::getInstance().addStream(stream_out);
  terrama2::core::Logger::getInstance().addStream(stream_file);
  terrama2::core::Logger::getInstance().initialize();

  auto exception = terrama2::core::DataAccessError() << terrama2::ErrorDescription("**Expected Error**");

  terrama2::core::Logger::getInstance() << exception;

  for(int i = 0; i < 50; ++i)
    terrama2::core::Logger::getInstance().info("Lorem Ipsummmm ");

}

void TsLogger::testUnload()
{

}

void TsLogger::cleanup()
{

}
