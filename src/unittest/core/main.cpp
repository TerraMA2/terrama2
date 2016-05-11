
// Qt
#include <QTimer>

//TerraMA2
#include <terrama2/core/utility/Utils.hpp>

// GoogleMock
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TsUtility.hpp"
#include "TsDataAccessorDcpInpe.hpp"
#include "TsDataRetrieverFTP.hpp"
#include "TsDataAccessorGeoTiff.hpp"

int main(int argc, char **argv)
{

  QCoreApplication app(argc, argv);


  ::testing::GTEST_FLAG(throw_on_failure) = true;
  ::testing::InitGoogleMock(&argc, argv);


  terrama2::core::initializeTerraMA();

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


  TsDataAccessorDcpInpe testDataAccessorDcpInpe;
  int ret = QTest::qExec(&testDataAccessorDcpInpe, argc, argv);

  //TsDataRetrieverFTP testDataRetrieverFTP;
  //int ret = QTest::qExec(&testDataRetrieverFTP, argc, argv);

  //TsDataAccessorGeoTiff testDataAccessorGeoTiff;
  //int ret = QTest::qExec(&testDataAccessorGeoTiff, argc, argv);

  terrama2::core::finalizeTerraMA();

  QTimer timer;
  QObject::connect(&timer, SIGNAL(timeout()), QCoreApplication::instance(), SLOT(quit()));
  timer.start(10000);
  app.exec();

  return ret;
}
