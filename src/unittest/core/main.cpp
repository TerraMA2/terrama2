//TerraMA2
#include "TsApplicationController.hpp"
#include "TsDataManager.hpp"
#include "Utils.hpp"


int main(int argc, char **argv)
{
  initializeTerraMA2();

  TsApplicationController testApplicationController;
  int ret = QTest::qExec(&testApplicationController, argc, argv);

  TsDataManager testDataProviderDAO;
  ret += QTest::qExec(&testDataProviderDAO, argc, argv);

  finalizeTerraMA2();

  return ret;
}
