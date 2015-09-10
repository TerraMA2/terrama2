//TerraMA2
#include "TestApplicationController.hpp"
#include "TestDataProviderDAO.hpp"
#include "TestDataSetDAO.hpp"

#include "TestUtils.hpp"


// QT
#include <QTest>

int main(int argc, char **argv)
{
  initializeTerraMA2();

  TestApplicationController testApplicationController;
  QTest::qExec(&testApplicationController, argc, argv);

  TestDataProviderDAO testDataProviderDAO;
  QTest::qExec(&testDataProviderDAO, argc, argv);

  TestDataSetDAO testDataSetDAO;
  QTest::qExec(&testDataSetDAO, argc, argv);

  finalizeTerraMA2();

  return 0;
}
