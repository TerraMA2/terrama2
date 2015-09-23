
#include "TestCollectorFile.hpp"

//Terrama2
#include <terrama2/collector/CollectorFile.hpp>
#include <terrama2/collector/Exception.hpp>

//Qt
#include <QTemporaryDir>

void TestCollectorFile::TestNormalBehavior()
{
  terrama2::core::DataProviderPtr dataProvider(new terrama2::core::DataProvider("dummy", terrama2::core::DataProvider::FILE_TYPE));

  try
  {
    terrama2::collector::CollectorFile collector(dataProvider);

    QCOMPARE(dataProvider, collector.dataProvider());
  }
  catch(...)
  {
    QFAIL("Should not be here");
  }

  return;
}

void TestCollectorFile::TestNullDataProvider()
{
  terrama2::core::DataProviderPtr nullDataProvider;

  try
  {
    terrama2::collector::CollectorFile invalidCollector(nullDataProvider);

    QFAIL("Should not be here");
  }
  catch(terrama2::collector::InvalidDataProviderException& e)
  {
    return;
  }
  catch(...)
  {
    QFAIL("Should not be here");
  }


  QFAIL("Should not be here");
}

void TestCollectorFile::TestWrongDataProviderKind()
{
  terrama2::core::DataProviderPtr dataProvider(new terrama2::core::DataProvider("dummy", terrama2::core::DataProvider::UNKNOWN_TYPE));

  try
  {
    terrama2::collector::CollectorFile invalidCollector(dataProvider);

    QFAIL("Should not be here");
  }
  catch(terrama2::collector::WrongDataProviderKindException& e)
  {
    return;
  }
  catch(...)
  {
    QFAIL("Should not be here");
  }


  QFAIL("Should not be here");
}

void TestCollectorFile::TestCheckConnection()
{
  QTemporaryDir tempDir;

  terrama2::core::DataProviderPtr dataProvider(new terrama2::core::DataProvider("dummy", terrama2::core::DataProvider::FILE_TYPE));
  dataProvider->setUri(tempDir.path().toStdString());

  try
  {
    terrama2::collector::CollectorFile collector(dataProvider);
    QVERIFY(collector.checkConnection());
  }
  catch(...)
  {
    QFAIL("Should not be here");
  }

  return;
}

QTEST_MAIN(TestCollectorFile)
//#include "TestCollectorFile.moc"
