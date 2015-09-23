//TerraMA2
#include "TsApplicationController.hpp"
#include "TsDataProviderDAO.hpp"
#include "TsDataSetDAO.hpp"

#include "Utils.hpp"


// QT
#include <QTest>

int main(int argc, char **argv)
{
  initializeTerraMA2();

  TsApplicationController testApplicationController;
  int ret = QTest::qExec(&testApplicationController, argc, argv);

  TsDataProviderDAO testDataProviderDAO;
  ret += QTest::qExec(&testDataProviderDAO, argc, argv);

  TsDataSetDAO testDataSetDAO;
  ret += QTest::qExec(&testDataSetDAO, argc, argv);

  finalizeTerraMA2();

  return ret;
}
