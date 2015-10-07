/*
  Copyright (C) 2007 National Institute For Space Research (INPE) - Brazil.

  This file is part of TerraMA2 - a free and open source computational
  platform for analysis, monitoring, and alert of geo-environmental extremes.

  TerraMA2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.

  TerraMA2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with TerraMA2. See LICENSE. If not, write to
  TerraMA2 Team at <terrama2-team@dpi.inpe.br>.
*/

/*!
  \file terrama2/collector/TsCollectorFile.cpp

  \brief Tests for the CollectorFile class.

  \author Jano Simas
*/

#include "TsCollectorFile.hpp"
#include "Utils.hpp"


//Terrama2
#include <terrama2/collector/CollectorFile.hpp>
#include <terrama2/collector/Exception.hpp>
#include <terrama2/core/DataSet.hpp>

//Qt
#include <QTemporaryDir>

void TsCollectorFile::TestNormalBehavior()
{
  QTemporaryDir dir;
  QTemporaryFile file(dir.path()+"/test_XXXXXX.csv");
  file.open();
  file.write("lat,lon,sat,data_pas\n");
  file.write("-10.7030,  30.3750,AQUA_M,2015-08-26 11:35:00\n");
  file.write("-10.7020,  30.3840,AQUA_M,2015-08-26 11:35:00\n");
  file.write("-10.4870,  30.4070,AQUA_M,2015-08-26 11:35:00\n");
  file.close();
  QFileInfo info(file);

  terrama2::core::DataProviderPtr dataProvider(new terrama2::core::DataProvider("dummy", terrama2::core::DataProvider::FILE_TYPE));
  dataProvider->setStatus(terrama2::core::DataProvider::ACTIVE);
  dataProvider->setUri(info.canonicalPath().toStdString());


  try
  {
    terrama2::collector::CollectorFile collector(dataProvider);

    QCOMPARE(dataProvider, collector.dataProvider());

    QVERIFY(collector.checkConnection());

    QVERIFY(collector.isOpen());
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
 
  return;
}

void TsCollectorFile::TestNullDataProvider()
{
  terrama2::core::DataProviderPtr nullDataProvider;

  try
  {
    terrama2::collector::CollectorFile invalidCollector(nullDataProvider);

    QFAIL(NO_EXCEPTION_THROWN);
  }
  catch(terrama2::collector::InvalidDataProviderError& e)
  {
    return;
  }
  catch(...)
  {
    QFAIL(WRONG_TYPE_EXCEPTION);
  }


  QFAIL(UNEXPECTED_BEHAVIOR);
}

void TsCollectorFile::TestInactiveDataSet()
{
  terrama2::core::DataProviderPtr dataProvider(new terrama2::core::DataProvider("dummy", terrama2::core::DataProvider::FILE_TYPE));

  try
  {
    terrama2::collector::CollectorFile collector(dataProvider);

    terrama2::core::DataSetPtr dataset = std::make_shared<terrama2::core::DataSet>(terrama2::core::DataSet(dataProvider,"dummy", terrama2::core::DataSet::PCD_TYPE));
    dataset->setStatus(terrama2::core::DataSet::INACTIVE);

    terrama2::collector::DataSetTimerPtr datasetTimer(new terrama2::collector::DataSetTimer(dataset));

    collector.collect(datasetTimer);

    QFAIL(NO_EXCEPTION_THROWN);
  }
  catch(terrama2::collector::InactiveDataSetError& e)
  {
    return;
  }
  catch(...)
  {
    QFAIL(WRONG_TYPE_EXCEPTION);
  }

  QFAIL(UNEXPECTED_BEHAVIOR);
}

void TsCollectorFile::TestWrongDataProviderKind()
{
  terrama2::core::DataProviderPtr dataProvider(new terrama2::core::DataProvider("dummy", terrama2::core::DataProvider::UNKNOWN_TYPE));

  try
  {
    terrama2::collector::CollectorFile invalidCollector(dataProvider);

    QFAIL(NO_EXCEPTION_THROWN);
  }
  catch(terrama2::collector::WrongDataProviderKindError& e)
  {
    return;
  }
  catch(...)
  {
    QFAIL(WRONG_TYPE_EXCEPTION);
  }


  QFAIL(UNEXPECTED_BEHAVIOR);
}

void TsCollectorFile::TestCheckConnection()
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
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

  return;
}

void TsCollectorFile::TestFailCheckConnection()
{
  terrama2::core::DataProviderPtr dataProvider(new terrama2::core::DataProvider("dummy", terrama2::core::DataProvider::FILE_TYPE));

  try
  {
    terrama2::collector::CollectorFile collector(dataProvider);
    QVERIFY(!collector.checkConnection());
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

  return;
}

//QTEST_MAIN(TsCollectorFile)
#include "TsCollectorFile.moc"
