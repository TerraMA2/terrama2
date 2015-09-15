
#include "TestCollectorOGR.hpp"

//Terrama2
#include <terrama2/core/DataProvider.hpp>
#include <terrama2/collector/CollectorOGR.hpp>
#include <terrama2/collector/Exception.hpp>

//terralib
#include <terralib/common/PlatformUtils.h>
#include <terralib/common.h>
#include <terralib/plugin.h>


void TestCollectorOGR::initTestCase()
{
  // Initialize the Terralib support
  TerraLib::getInstance().initialize();

  te::plugin::PluginInfo* info;
  std::string plugins_path = te::common::FindInTerraLibPath("share/terralib/plugins");

  info = te::plugin::GetInstalledPlugin(plugins_path + "/te.da.ogr.teplg");
  te::plugin::PluginManager::getInstance().add(info);

  te::plugin::PluginManager::getInstance().loadAll();
}

void TestCollectorOGR::cleanupTestCase()
{
  TerraLib::getInstance().finalize();
}

void TestCollectorOGR::testInvalidDataProvider()
{
  terrama2::core::DataProviderPtr dataProvider(new terrama2::core::DataProvider("dummy"));

  try
  {
    terrama2::collector::CollectorOGR collector(dataProvider);

    QFAIL("Exception not raised");
  }
  catch(const terrama2::collector::WrongDataProviderKindException& e)
  {
    return;
  }
  catch(...)
  {
    QFAIL("Should not be here!!");
  }

  QFAIL("Should not be here!!");
}

QTEST_MAIN(TestCollectorOGR)
#include "TestCollectorOGR.moc"
