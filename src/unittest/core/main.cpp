
// Qt
#include <QTimer>

//TerraMA2
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>

// GoogleMock
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TsUtility.hpp"
#include "TsLogger.hpp"
#include "TsDataRetrieverFTP.hpp"
#include "TsDataAccessorDcpInpe.hpp"
#include "TsDataAccessorDcpToa5.hpp"


int main(int argc, char** argv)
{
  int ret = 0;
  QCoreApplication app(argc, argv);

  ::testing::GTEST_FLAG(throw_on_failure) = true;
  ::testing::InitGoogleMock(&argc, argv);

  terrama2::core::initializeTerraMA();

  terrama2::core::disableLogger();

  try
  {
    TsUtility testUtility;
    ret += QTest::qExec(&testUtility, argc, argv);
  }
  catch(...)
  {

  }

  try
  {
    TsLogger testLogger;
    ret += QTest::qExec(&testLogger, argc, argv);
  }
  catch(...)
  {

  }

  try
  {
    TsDataRetrieverFTP testDataRetrieverFTP;
    ret += QTest::qExec(&testDataRetrieverFTP, argc, argv);
  }
  catch(...)
  {

  }

  try
  {
    TsDataAccessorDcpInpe testDataAccessorDcpInpe;
    ret += QTest::qExec(&testDataAccessorDcpInpe, argc, argv);
  }
  catch(...)
  {

  }

  try
  {
    TsDataAccessorDcpToa5 testDataAccessorDcpToa5;
    ret += QTest::qExec(&testDataAccessorDcpToa5, argc, argv);
  }
  catch(...)
  {

  }

  terrama2::core::finalizeTerraMA();

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(10000);
  app.exec();

  return ret;
}
