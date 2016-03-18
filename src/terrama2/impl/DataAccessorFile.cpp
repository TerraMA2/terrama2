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
#include "../core/utility/Raii.hpp"

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


std::string terrama2::core::DataAccessorFile::getMask(const DataSet& dataset) const
{
  try
  {
    return dataset.format.at("mask");
  }
  catch (...)
  {
    //TODO: log this
    //TODO: throw UndefinedTag
    throw;
  }
}

std::string terrama2::core::DataAccessorFile::retrieveData(const DataRetrieverPtr dataRetriever, const DataSet& dataset, const Filter& filter) const
{
  std::string mask = getMask(dataset);
  return dataRetriever->retrieveData(mask, filter);
}

 std::shared_ptr<te::mem::DataSet> terrama2::core::DataAccessorFile::getDataSet(const std::string& uri, const Filter& filter, const DataSet& dataSet) const
 {
   QUrl url(uri.c_str());
   QDir dir(url.path());
   QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable | QDir::CaseSensitive);
   if(fileInfoList.empty())
   {
     //TODO: log here
     //TODO: throw here
     return nullptr;
   }

   bool first = true;
   std::shared_ptr<te::mem::DataSet> completeDataset(nullptr);
   std::shared_ptr<te::da::DataSetTypeConverter> converter(nullptr);
   for(const auto& fileInfo : fileInfoList)
   {
     std::string name = fileInfo.fileName().toStdString();
     std::string baseName = fileInfo.baseName().toStdString();
     // Verify if the file name matches the mask
     if(!isValidDataSetName(getMask(dataSet), filter, name))
       continue;

     // creates a DataSource to the data and filters the dataset,
     // also joins if the DCP comes from separated files
     std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make(dataSourceTye()));
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

     if(first)
     {
     //read and adapt all te:da::DataSet from terrama2::core::DataSet
       converter = getConverter(dataSet, std::shared_ptr<te::da::DataSetType>(transactor->getDataSetType(baseName)));
       std::shared_ptr<te::da::DataSetType> datasetType(static_cast<te::da::DataSetType*>(converter->getResult()->clone()));
       assert(datasetType);
       completeDataset = std::make_shared<te::mem::DataSet>(datasetType.get());
       first = false;
     }

     assert(converter);
     std::unique_ptr<te::da::DataSet> datasetOrig(transactor->getDataSet(baseName));
     assert(datasetOrig);
     std::shared_ptr<te::da::DataSet> dataset(te::da::CreateAdapter(datasetOrig.release(), converter.get(), true));

     //TODO:.. filter and join te::da::dataset from each dataset
     //TODO: join dataset
     completeDataset->copy(*dataset);
   }

   return completeDataset;
 }
