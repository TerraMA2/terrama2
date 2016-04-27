//TerraMA2
#include <terrama2/core/utility/Utils.hpp>

#include "TsJSONUtils.hpp"


int main(int argc, char **argv)
{
  terrama2::core::initializeTerraMA();

  TsJSONUtils testJSONUtils;
  int ret = QTest::qExec(&testJSONUtils, argc, argv);


  terrama2::core::finalizeTerraMA();

  return ret;
}
