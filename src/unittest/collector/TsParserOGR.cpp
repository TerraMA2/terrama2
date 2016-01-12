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
#include "Mock.hpp"

//terrama2
#include <terrama2/collector/DataFilter.hpp>
#include <terrama2/collector/ParserOGR.hpp>
#include <terrama2/collector/Exception.hpp>
#include <terrama2/core/Utils.hpp>

//Terralib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/common/Exception.h>

//QT
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QUrl>

//std
#include <iostream>

void TsParserOGR::TestEmptyFile()
{
  QTemporaryDir dir;
  QTemporaryFile file(dir.path()+"/test_XXXXXX.csv");
  file.open();
  file.close();
  QFileInfo info(file);

  QUrl uri;
  uri.setScheme("FILE");
  uri.setPath(info.absoluteFilePath());

  try
  {
    terrama2::collector::TransferenceData transferenceData;
    transferenceData.uri_temporary = uri.url().toStdString();

    std::vector<terrama2::collector::TransferenceData> transferenceDataVec;
    transferenceDataVec.push_back(transferenceData);

    terrama2::core::DataSetItem item;
    item.setMask(info.fileName().toStdString());

    terrama2::collector::DataFilterPtr filter = std::make_shared<terrama2::collector::DataFilter>(item);

    terrama2::collector::ParserOGR parser;
    parser.read(filter, transferenceDataVec);

    QFAIL(UNEXPECTED_BEHAVIOR);
  }
  catch(terrama2::collector::UnableToReadDataSetException& e)
  {
    return;
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(WRONG_TYPE_EXCEPTION);
  }
  catch(...)
  {
    QFAIL(WRONG_TYPE_EXCEPTION);
  }

  QFAIL(UNEXPECTED_BEHAVIOR);
}

void TsParserOGR::TestCsvFile()
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

  QUrl uri;
  uri.setScheme("FILE");
  uri.setPath(info.absoluteFilePath());


  try
  {
    terrama2::core::DataSetItem item;
    item.setMask(info.fileName().toStdString());

    terrama2::collector::DataFilterPtr filter = std::make_shared<terrama2::collector::DataFilter>(item);

    terrama2::collector::TransferenceData transferenceData;
    transferenceData.uri_temporary = uri.url().toStdString();

    std::vector<terrama2::collector::TransferenceData> transferenceDataVec;
    transferenceDataVec.push_back(transferenceData);

    terrama2::collector::ParserOGR parser;
    parser.read(filter, transferenceDataVec);

    QVERIFY(transferenceDataVec.size() == 1);
    //TODO: test datasettype
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(NO_EXCEPTION_EXPECTED);
  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

  return;
}

void TsParserOGR::initTestCase()
{
}

void TsParserOGR::cleanupTestCase()
{
}

void TsParserOGR::TestInvalidFolder()
{
  try
  {
    terrama2::core::DataSetItem item;
    item.setMask("dummy");

    terrama2::collector::DataFilterPtr filter = std::make_shared<terrama2::collector::DataFilter>(item);

    terrama2::collector::TransferenceData transferenceData;
    transferenceData.uri_temporary = "__DUMMY__";

    std::vector<terrama2::collector::TransferenceData> transferenceDataVec;
    transferenceDataVec.push_back(transferenceData);

    terrama2::collector::ParserOGR parser;
    parser.read(filter, transferenceDataVec);

    QFAIL(NO_EXCEPTION_THROWN);
  }
  catch(terrama2::collector::InvalidFolderException& e)
  {

  }
  catch(...)
  {
    QFAIL(WRONG_TYPE_EXCEPTION);
  }
}

