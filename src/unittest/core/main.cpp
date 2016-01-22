//TerraMA2
#include "TsApplicationController.hpp"
#include "TsDataManager.hpp"
#include "TsLogger.hpp"
#include "Utils.hpp"
#include "TsSerializer.hpp"


int main(int argc, char **argv)
{
  initializeTerraMA2();

  TsApplicationController testApplicationController;
  int ret = QTest::qExec(&testApplicationController, argc, argv);

  TsSerializer testSerializer;
  ret += QTest::qExec(&testSerializer, argc, argv);

  TsDataManager testDataManager;
  ret += QTest::qExec(&testDataManager, argc, argv);

  TsLogger testLogger;
  ret = QTest::qExec(&testLogger, argc, argv);

  finalizeTerraMA2();

  return ret;
}
