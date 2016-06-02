
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


int main(int argc, char **argv)
{
  int ret = 0;
  QCoreApplication app(argc, argv);

  ::testing::GTEST_FLAG(throw_on_failure) = true;
  ::testing::InitGoogleMock(&argc, argv);

  terrama2::core::initializeTerralib();

  terrama2::core::disableLogger();

 // TsUtility testUtility;
 // ret += QTest::qExec(&testUtility, argc, argv);

  TsLogger testLogger;
  ret += QTest::qExec(&testLogger, argc, argv);

  TsDataRetrieverFTP testDataRetrieverFTP;
  ret += QTest::qExec(&testDataRetrieverFTP, argc, argv);

  terrama2::core::finalizeTerralib();

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(10000);
  app.exec();

  return ret;
}
