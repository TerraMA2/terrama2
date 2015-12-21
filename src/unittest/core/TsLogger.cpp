#include "TsLogger.hpp"

#include <terrama2/core/Logger.hpp>
#include <terrama2/core/Exception.hpp>


void TsLogger::init()
{

}

void TsLogger::testLoad()
{
  terrama2::core::Logger::getInstance().addStream("/home/raphael/Documents/tmp/terrama2.log");
  terrama2::core::Logger::getInstance().initialize();

  auto exception = terrama2::core::DataAccessError() << terrama2::ErrorDescription("**Expected DataAccess Error**");

  for(int i = 0; i < 20; ++i)
    TERRAMA2_LOG_TRACE() << "Trace Message Lorem ipsu " + std::to_string(i);

  // logging an exception
  TERRAMA2_LOG_ERROR() << exception;
}

void TsLogger::testUnload()
{

}

void TsLogger::cleanup()
{

}
