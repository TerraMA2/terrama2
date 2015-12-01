#include "TsLogger.hpp"

#include <terrama2/core/Logger.hpp>


void TsLogger::init()
{

}

void TsLogger::testLoad()
{
  terrama2::core::Logger log("/home/raphael/Documents/tmp/logcore.log");

  log.write("Fun");
}

void TsLogger::testUnload()
{

}

void TsLogger::cleanup()
{

}
