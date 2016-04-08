//TerraMA2
#include <terrama2/core/utility/Utils.hpp>

#include "TsUtility.hpp"


int main(int argc, char **argv)
{
  terrama2::core::initializeTerraMA();

  TsUtility testUtility;
  int ret = QTest::qExec(&testUtility, argc, argv);

  //  TsApplicationController testApplicationController;
  //  int ret = QTest::qExec(&testApplicationController, argc, argv);

  //  TsSerializer testSerializer;
  //  ret += QTest::qExec(&testSerializer, argc, argv);

  //  TsDataManager testDataManager;
  //  ret += QTest::qExec(&testDataManager, argc, argv);

  //  TsLogger testLogger;
  //  ret = QTest::qExec(&testLogger, argc, argv);


  terrama2::core::finalizeTerraMA();

  return ret;
}
