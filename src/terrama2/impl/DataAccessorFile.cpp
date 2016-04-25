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
  \file terrama2/core/data-access/DataAccessorFile.cpp

  \brief

  \author Jano Simas
 */

#include "DataAccessorFile.hpp"
#include "../core/utility/FilterUtils.hpp"
#include "../core/utility/Logger.hpp"
#include "../core/utility/Raii.hpp"

//STL
#include <algorithm>

//QT
#include <QUrl>
#include <QDir>

//terralib
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/dataset/DataSetTypeConverter.h>
#include <terralib/dataaccess/dataset/DataSetAdapter.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/datatype/DateTimeProperty.h>


std::string terrama2::core::DataAccessorFile::getMask(DataSetPtr dataSet) const
{
  try
  {
    return dataSet->format.at("mask");
  }
  catch (...)
  {
    QString errMsg = QObject::tr("Undefined mask in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw UndefinedTagException() << ErrorDescription(errMsg);
  }
}

std::string terrama2::core::DataAccessorFile::retrieveData(const DataRetrieverPtr dataRetriever, DataSetPtr dataset, const Filter& filter) const
{
  std::string mask = getMask(dataset);
  return dataRetriever->retrieveData(mask, filter);
}

terrama2::core::Series terrama2::core::DataAccessorFile::getSeries(const std::string& uri,
                                                                   const terrama2::core::Filter& filter,
                                                                   terrama2::core::DataSetPtr dataSet) const
{
  QUrl url(uri.c_str());
  QDir dir(url.path());
  QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable | QDir::CaseSensitive);
  if(fileInfoList.empty())
  {
    QString errMsg = QObject::tr("No file in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw NoDataException() << ErrorDescription(errMsg);
  }

  //return value
  Series series;
  series.dataSet = dataSet;

  std::shared_ptr<te::mem::DataSet> completeDataset(nullptr);
  std::shared_ptr<te::da::DataSetTypeConverter> converter(nullptr);

  bool first = true;
  for(const auto& fileInfo : fileInfoList)
  {
    std::string name = fileInfo.fileName().toStdString();
    std::string baseName = fileInfo.baseName().toStdString();
    // Verify if the file name matches the mask
    // FIXME: use timestamp
     std::shared_ptr< te::dt::TimeInstantTZ > timestamp;
     if(!isValidDataSetName(getMask(dataSet), filter, name,timestamp))
       continue;

    // creates a DataSource to the data and filters the dataset,
    // also joins if the DCP comes from separated files
    std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make(dataSourceType()));
    std::map<std::string, std::string> connInfo;

    connInfo["URI"] = typePrefix() + dir.absolutePath().toStdString() + "/" + name;
    datasource->setConnectionInfo(connInfo);

    //RAII for open/closing the datasource
    OpenClose<std::shared_ptr<te::da::DataSource> > openClose(datasource);

    if(!datasource->isOpened())
    {
      QString errMsg = QObject::tr("DataProvider could not be opened.");
      TERRAMA2_LOG_ERROR() << errMsg;

      continue;
    }

    // get a transactor to interact to the data source
    std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());

    // Some drivers use tha base name and other use filename with extension
    std::string dataSetName;
    std::vector<std::string> dataSetNames = transactor->getDataSetNames();
    auto itBaseName = std::find(dataSetNames.cbegin(), dataSetNames.cend(), baseName);
    auto itFileName = std::find(dataSetNames.cbegin(), dataSetNames.cend(), name);
    if(itBaseName != dataSetNames.cend())
      dataSetName = baseName;
    else if(itFileName != dataSetNames.cend())
      dataSetName = name;
    //No valid dataset name found
    if(dataSetName.empty())
      continue;

    if(first)
    {
      //read and adapt all te:da::DataSet from terrama2::core::DataSet
      converter = getConverter(dataSet, std::shared_ptr<te::da::DataSetType>(transactor->getDataSetType(dataSetName)));
      series.teDataSetType.reset(static_cast<te::da::DataSetType*>(converter->getResult()->clone()));
      assert(series.teDataSetType.get());
      completeDataset = std::make_shared<te::mem::DataSet>(series.teDataSetType.get());
      first = false;
    }

    assert(converter);
    std::unique_ptr<te::da::DataSet> datasetOrig(transactor->getDataSet(dataSetName));
    assert(datasetOrig);
    std::shared_ptr<te::da::DataSet> teDataSet(te::da::CreateAdapter(datasetOrig.release(), converter.get(), true));

    //FIXME: Nor working with raster!!

    completeDataset->copy(*teDataSet);

    if(completeDataset->isEmpty())
    {
      QString errMsg = QObject::tr("No data in dataset: %1.").arg(dataSet->id);
      TERRAMA2_LOG_WARNING() << errMsg;
    }
  }// for each file

  if(!completeDataset.get())
  {
    QString errMsg = QObject::tr("No data in dataset: %1.").arg(dataSet->id);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::NoDataException() << ErrorDescription(errMsg);
  }

  std::shared_ptr<SyncronizedDataSet> syncDataset(new SyncronizedDataSet(completeDataset));
  series.syncDataSet = syncDataset;
  return series;
}
