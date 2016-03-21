//TerraMA2
#include "TsPythonInterpreter.hpp"

#include <terrama2/core/Utils.hpp>
#include <terrama2/analysis/core/PythonInterpreter.hpp>

#include <QTimer>
#include <QCoreApplication>

int main(int argc, char **argv)
{

  QCoreApplication app(argc, argv);

  terrama2::core::initializeTerralib();

  terrama2::analysis::core::init();

  TsPythonInterpreter testPythonInterpreter;
  int ret = QTest::qExec(&testPythonInterpreter, argc, argv);

  terrama2::analysis::core::finalize();

  terrama2::core::finalizeTerralib();

  return ret;
}
