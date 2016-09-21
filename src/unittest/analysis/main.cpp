//TerraMA2
#include <terrama2/core/utility/Utils.hpp>
#include <terrama2/core/utility/TerraMA2Init.hpp>

#include "TsJSONUtils.hpp"


int main(int argc, char **argv)
{
  terrama2::core::TerraMA2Init terramaRaii;

  TsJSONUtils testJSONUtils;
  int ret = QTest::qExec(&testJSONUtils, argc, argv);


  

  return ret;
}
