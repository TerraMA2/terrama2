
// Qt
#include <QTimer>

//TerraMA2
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/SemanticsManager.hpp>

// GoogleMock
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TsUtility.hpp"
#include "TsDataRetrieverFTP.hpp"


int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  ::testing::GTEST_FLAG(throw_on_failure) = true;
  ::testing::InitGoogleMock(&argc, argv);

  terrama2::core::initializeTerralib();

  terrama2::core::disableLogger();

  //  TsUtility testUtility;
  //  int ret = QTest::qExec(&testUtility, argc, argv);

  //  TsApplicationController testApplicationController;
  //  int ret = QTest::qExec(&testApplicationController, argc, argv);

  //  TsSerializer testSerializer;
  //  ret += QTest::qExec(&testSerializer, argc, argv);

  //  TsDataManager testDataManager;
  //  ret += QTest::qExec(&testDataManager, argc, argv);

  //  TsLogger testLogger;
  //  ret = QTest::qExec(&testLogger, argc, argv);

  TsDataRetrieverFTP testDataRetrieverFTP;
  int ret = QTest::qExec(&testDataRetrieverFTP, argc, argv);

  terrama2::core::finalizeTerralib();

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(10000);
  app.exec();

  return ret;
}
