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

  TERRAMA2_LOG_ERROR() << "**TYPE ERROR EXPECTED**";

  // logging an exception
  TERRAMA2_LOG_ERROR() << exception;
}

void TsLogger::testUnload()
{

}

void TsLogger::cleanup()
{

}
