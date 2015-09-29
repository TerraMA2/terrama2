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
  \file terrama2/collector/TsParserOGR.hpp

  \brief Tests for the ParserOGR class.

  \author Jano Simas
*/

#include "TsParserOGR.hpp"
#include "Utils.hpp"

//terrama2
#include <terrama2/collector/ParserOGR.hpp>
#include <terrama2/collector/Exception.hpp>

//Terralib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/dataset/DataSet.h>

//QT
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QFileInfo>

void TsParserOGR::TestNullDataSource()
{
  QFAIL("Not implemented");
}

void TsParserOGR::TestDataSourceNotOpen()
{
  QFAIL("Not implemented");
}

void TsParserOGR::TestEmptyFile()
{
  initializeTerralib();

  QTemporaryDir dir;
  QTemporaryFile file(dir.path()+"/test_XXXXXX.csv");
  file.open();
  file.close();
  QFileInfo info(file);

  try
  {
    terrama2::collector::ParserOGR parser;
    std::vector<std::string> names { info.baseName().toStdString() };

    std::vector<std::shared_ptr<te::da::DataSet>> datasetVec;
    std::vector<std::shared_ptr<te::da::DataSetType>> datasetTypeVec;
    parser.read(dir.path().toStdString(), names, datasetVec, datasetTypeVec);

    QFAIL("Should not be here.");
  }
  catch(terrama2::collector::UnableToReadDataSetError e)
  {
    finalizeTerralib();
    return;
  }
  catch(...)
  {
    QFAIL("Unknown Exception...Should not be here.");
  }

  QFAIL("End of test... Should not be here.");
}

//QTEST_MAIN(TsParserOGR)
#include "TsParserOGR.moc"
