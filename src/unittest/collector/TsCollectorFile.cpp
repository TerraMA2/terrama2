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

//Terrama2
#include <terrama2/collector/CollectorFile.hpp>
#include <terrama2/collector/Exception.hpp>

//Qt
#include <QTemporaryDir>

void TsCollectorFile::TestNormalBehavior()
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

void TsCollectorFile::TestNullDataProvider()
{
  terrama2::core::DataProviderPtr nullDataProvider;

  try
  {
    terrama2::collector::CollectorFile invalidCollector(nullDataProvider);

    QFAIL("Should not be here");
  }
  catch(terrama2::collector::InvalidDataProviderError& e)
  {
    return;
  }
  catch(...)
  {
    QFAIL("Should not be here");
  }


  QFAIL("Should not be here");
}

void TsCollectorFile::TestWrongDataProviderKind()
{
  terrama2::core::DataProviderPtr dataProvider(new terrama2::core::DataProvider("dummy", terrama2::core::DataProvider::UNKNOWN_TYPE));

  try
  {
    terrama2::collector::CollectorFile invalidCollector(dataProvider);

    QFAIL("Should not be here");
  }
  catch(terrama2::collector::WrongDataProviderKindError& e)
  {
    return;
  }
  catch(...)
  {
    QFAIL("Should not be here");
  }


  QFAIL("Should not be here");
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
    QFAIL("Should not be here");
  }

  return;
}

//QTEST_MAIN(TsCollectorFile)
#include "TsCollectorFile.moc"
