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

//std
#include <iostream>

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
    std::shared_ptr<te::da::DataSetType>          datasetType;
    parser.read(info.absolutePath().toStdString(), names, datasetVec, datasetType);

    QFAIL("Should not be here.");
  }
  catch(terrama2::collector::UnableToReadDataSetError& e)
  {
    return;
  }
  catch(te::common::Exception& e)
  {
    QFAIL("Teralib Exception...Should not be here.");
  }
  catch(...)
  {
    QFAIL("Unknown Exception...Should not be here.");
  }

  QFAIL("End of test... Should not be here.");
}

void TsParserOGR::TestDataSetNames()
{
  QTemporaryDir dir;
  QString baseName("test_XXXXXX");
  QTemporaryFile file(dir.path()+"/" + baseName + ".csv");
  file.open();
  file.write("lat,lon,sat,data_pas\n");
  file.write("-10.7030,  30.3750,AQUA_M,2015-08-26 11:35:00\n");
  file.write("-10.7020,  30.3840,AQUA_M,2015-08-26 11:35:00\n");
  file.write("-10.4870,  30.4070,AQUA_M,2015-08-26 11:35:00\n");
  file.close();
  QFileInfo info(file);

  terrama2::collector::ParserOGR parser;
  std::vector<std::string> datasetNames = parser.datasetNames(info.absolutePath().toStdString());

  QVERIFY(datasetNames.size() == 1);
  QCOMPARE(datasetNames.at(0) , baseName.toStdString());
}

void TsParserOGR::TestInpeCsvFile()
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

  try
  {
    terrama2::collector::ParserOGR parser;
    std::vector<std::string> names { info.baseName().toStdString() };

    std::vector<std::shared_ptr<te::da::DataSet>> datasetVec;
    std::shared_ptr<te::da::DataSetType>          datasetType;
    parser.read(info.absolutePath().toStdString(), names, datasetVec, datasetType);

    QVERIFY(datasetVec.size() == 1);
    //TODO: test datasettype
  }
  catch(terrama2::collector::UnableToReadDataSetError& e)
  {
    QFAIL(e.what());
  }
  catch(te::common::Exception& e)
  {
    QFAIL("Teralib Exception...Should not be here.");
  }
  catch(...)
  {
    QFAIL("Unknown Exception...Should not be here.");
  }

  return;
}

void TsParserOGR::readCSV()
{
  try
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
    std::string path = info.absoluteFilePath().toStdString();
//    std::string path = terrama2::core::FindInTerraMA2Path("src/unittest/collector/data/pcd_inpe.csv");
    qDebug() << path.c_str();
//    std::string path = "/home/jano/projetos/inpe/testData/shapefile/BRA_adm0.shp";

    std::map<std::string, std::string> connInfo;

    connInfo["URI"] = path;

    std::auto_ptr<te::da::DataSource> ds = te::da::DataSourceFactory::make("OGR");
    ds->setConnectionInfo(connInfo);
    ds->open();

    auto dsNames = ds->getDataSetNames();
    std::string datasetName = dsNames[0];

    auto dataset = ds->getDataSet(datasetName);

    // let's get the dataset structure

    std::size_t nproperties = dataset->getNumProperties();

    // this will be used just to count the items in the dataset
    int item = 0;

    std::cout << "Printing information about the dataset: " << datasetName << std::endl;

    // traverse the dataset and print each dataset item
    while(dataset->moveNext())
    {
      std::cout << std::endl << "ITEM NUMBER: " << item++ << " =======================" << std::endl;

      for(std::size_t i = 0; i < nproperties; ++i)
      {
        std::cout << dataset->getPropertyName(i) << ": " ;

        // check if value is not null
        if(dataset->isNull(i))
        {
          std::cout << std::endl;
          continue;
        }

        // get data value
        std::string value = dataset->getAsString(i);

        std::cout << value << std::endl;
      }
    }

    dataset.release();

    ds->close();

    ds.release();

    // release the data source: you are the owner
    //delete ds; //it is auto_ptr
  }
  catch(const te::common::Exception& e)
  {
    std::cout << std::endl << "An terralib exception has occurred in the OGR Example: " << e.what() << std::endl;
  }
  catch(const std::exception& e)
  {
    std::cout << std::endl << "An exception has occurred in the OGR Example: " << e.what() << std::endl;
  }
  catch(...)
  {
    std::cout << std::endl << "An unexpected exception has occurred in the OGR Example!" << std::endl;
  }

}

void TsParserOGR::initTestCase()
{
}

void TsParserOGR::cleanupTestCase()
{
}

//QTEST_MAIN(TsParserOGR)
#include "TsParserOGR.moc"
