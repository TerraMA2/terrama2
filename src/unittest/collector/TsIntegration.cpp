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
  \file terrama2/collector/TsIntegration.cpp

  \brief Integrated tests for collector module.

  \author Jano Simas
*/

#include "TsIntegration.hpp"

//terrama
#include <terrama2/collector/CollectorService.hpp>
#include <terrama2/collector/Exception.hpp>
#include <terrama2/core/DataProvider.hpp>
#include <terrama2/core/DataSetItem.hpp>
#include <terrama2/core/DataSet.hpp>

//terralib
#include <terralib/datatype/TimeDuration.h>
#include <terralib/common/Exception.h>

//QT
#include <QTimer>

void TsIntegration::TestReadCsvStorePostGis()
{
  QTemporaryDir dir;
  QTemporaryFile file(dir.path()+"/test_XXXXXX.csv");
  file.open();
  file.close();
  QFileInfo info(file);

  std::vector<std::string> names { info.baseName().toStdString() };

  try
  {
    terrama2::core::DataProviderPtr provider(new terrama2::core::DataProvider("TestProvider", terrama2::core::DataProvider::FILE_TYPE));
    provider->setStatus(terrama2::core::DataProvider::ACTIVE);
    provider->setUri(info.canonicalPath().toStdString());

    terrama2::core::DataSetPtr dataset(new terrama2::core::DataSet(provider, "TestDataSet", terrama2::core::DataSet::PCD_TYPE));
    dataset->setStatus(terrama2::core::DataSet::ACTIVE);

    te::dt::TimeDuration frequency(0, 0, 5);
    dataset->setDataFrequency(frequency);

    terrama2::core::DataSetItemPtr item(new terrama2::core::DataSetItem(dataset, terrama2::core::DataSetItem::PCD_INPE_TYPE));

    std::vector<terrama2::core::DataSetItemPtr> datasetItemVect = {item};
    dataset->setDataSetItemList(datasetItemVect);

    std::vector<terrama2::core::DataSetPtr> datasetVec = {dataset};
    provider->setDataSets(datasetVec);

    terrama2::collector::CollectorService service;
    service.addDataset(dataset);

    service.start();

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), QApplication::instance(), SLOT(quit()));
    timer.start(120000);

    QApplication::exec();
  }
  catch(terrama2::Exception& e)
  {
    qDebug() << e.what();
    QFAIL("Terrama2 exception...");
  }
  catch(te::common::Exception& e)
  {
    qDebug() << e.what();
    QFAIL("Terralib exception...");
  }
  catch(...)
  {
    QFAIL("Unkown exception...");
  }
}



//QTEST_MAIN(TsIntegration)
#include "TsIntegration.moc"
