//TerraMA2
#include "TestApplicationController.hpp"
#include "TestDataProviderDAO.hpp"
#include "TestDataSetDAO.hpp"

#include "Utils.hpp"


// QT
#include <QTest>

int main(int argc, char **argv)
{
  initializeTerraMA2();

  TestApplicationController testApplicationController;
  int ret = QTest::qExec(&testApplicationController, argc, argv);

  TestDataProviderDAO testDataProviderDAO;
  ret += QTest::qExec(&testDataProviderDAO, argc, argv);

  TestDataSetDAO testDataSetDAO;
  ret += QTest::qExec(&testDataSetDAO, argc, argv);

  finalizeTerraMA2();

  return ret;
}
