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
  \file terrama2/collector/TestParserOGR.hpp

  \brief Tests for the ParserOGR class.

  \author Jano Simas
*/

#include "TestParserOGR.hpp"

//terrama2
#include <terrama2/collector/ParserOGR.hpp>

//Terralib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/dataset/DataSet.h>

//QT
#include <QTemporaryDir>
#include <QTemporaryFile>

void TestParserOGR::TestNullDataSource()
{
  QFAIL("Not implemented");
}

void TestParserOGR::TestDataSourceNotOpen()
{
  QFAIL("Not implemented");
}

void TestParserOGR::TestEmptyFile()
{
  QTemporaryDir dir;
  QTemporaryFile file(dir.isValid()+"/test_XXXXXX");

  try
  {
    terrama2::collector::ParserOGR parser;
    std::shared_ptr<te::da::DataSet> dataset = parser.read(dir.path().toStdString(), QStringList() << file.fileName());

    QVERIFY(dataset->isEmpty());
  }
  catch(...)
  {
    QFAIL("Read Exception...Should not be here.");
  }

}

QTEST_MAIN(TestParserOGR)
#include "TestParserOGR.moc"
