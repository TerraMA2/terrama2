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
  \file terrama2/services/view/core/data-access/DataAccess.cpp

  \brief Access data through TerraLib

  \author Vinicius Campanha
*/


// TerraMA2
#include "DataAccess.hpp"
#include "Exception.hpp"
#include "../../../../impl/DataAccessorFile.hpp"
#include "../../../../impl/DataAccessorPostGIS.hpp"
#include "../../../../core/data-model/DataProvider.hpp"
#include "../../../../core/utility/DataAccessorFactory.hpp"
#include "../../../../core/utility/Raii.hpp"
#include "../../../../core/utility/Utils.hpp"

// TerraLib
#include "terralib/dataaccess/datasource/DataSourceFactory.h"
#include "terralib/dataaccess/datasource/DataSource.h"

//Qt

// STL



QFileInfoList terrama2::services::view::core::DataAccess::getFilesList(const std::pair< terrama2::core::DataSeriesPtr, terrama2::core::DataProviderPtr >& dataSeriesProvider,
                                                                       const terrama2::core::DataSetPtr dataSet,
                                                                       const terrama2::core::Filter& filter)
{
  terrama2::core::DataSeriesPtr inputDataSeries = dataSeriesProvider.first;
  terrama2::core::DataProviderPtr inputDataProvider = dataSeriesProvider.second;

  terrama2::core::DataAccessorPtr dataAccessor =
      terrama2::core::DataAccessorFactory::getInstance().make(inputDataProvider, inputDataSeries);

  std::shared_ptr< terrama2::core::DataAccessorFile > dataAccessorFile =
      std::dynamic_pointer_cast<terrama2::core::DataAccessorFile>(dataAccessor);

  std::string timezone;
  try
  {
    timezone = dataAccessorFile->getTimeZone(dataSet);
  }
  catch(const terrama2::core::UndefinedTagException& /*e*/)
  {
    //if timezone is not defined
    timezone = "UTC+00";
  }

  auto remover = std::make_shared<terrama2::core::FileRemover>();

  // Get the list of layers to register
  auto fileInfoList = dataAccessorFile->getFilesList(inputDataProvider->uri,
                                                     terrama2::core::getFileMask(dataSet),
                                                     terrama2::core::getFolderMask(dataSet),
                                                     filter,
                                                     timezone,
                                                     remover);

  return fileInfoList;
}

std::unique_ptr< te::da::DataSetType > terrama2::services::view::core::DataAccess::getVectorialDataSetType(const QFileInfo& fileInfo)
{
  return getDataSetType("file://"+fileInfo.absoluteFilePath().toStdString(),
                       fileInfo.baseName().toStdString(),
                       "OGR");
}

std::unique_ptr< te::da::DataSetType > terrama2::services::view::core::DataAccess::getGeotiffDataSetType(const QFileInfo& fileInfo)
{
  return getDataSetType("file://"+fileInfo.absoluteFilePath().toStdString(),
                       fileInfo.fileName().toStdString(),
                       "GDAL");
}

std::unique_ptr< te::da::DataSetType > terrama2::services::view::core::DataAccess::getDataSetType(const std::string& dataSourceURI,
                                                                                const std::string& dataSetName,
                                                                                const std::string& driver)
{
  std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make(driver,
                                                                                 dataSourceURI));

  terrama2::core::OpenClose<std::shared_ptr<te::da::DataSource>> openClose(datasource);
  if(!datasource->isOpened())
  {
    QString errMsg = QObject::tr("DataProvider could not be opened.");
    TERRAMA2_LOG_ERROR() << errMsg;
    return nullptr;
  }

  return std::unique_ptr< te::da::DataSetType >(datasource->getDataSetType(dataSetName));
}


terrama2::services::view::core::TableInfo terrama2::services::view::core::DataAccess::getPostgisTableInfo(const std::pair<terrama2::core::DataSeriesPtr, terrama2::core::DataProviderPtr>& dataSeriesProvider,
                                                                            const terrama2::core::DataSetPtr dataSet)
{
  terrama2::core::DataSeriesPtr inputDataSeries = dataSeriesProvider.first;
  terrama2::core::DataProviderPtr inputDataProvider = dataSeriesProvider.second;

  terrama2::core::DataAccessorPtr dataAccessor =
      terrama2::core::DataAccessorFactory::getInstance().make(inputDataProvider, inputDataSeries);

  std::shared_ptr< terrama2::core::DataAccessorPostGIS > dataAccessorPostGis =
      std::dynamic_pointer_cast<terrama2::core::DataAccessorPostGIS>(dataAccessor);

  TableInfo tableInfo;
  tableInfo.tableName = dataAccessorPostGis->getDataSetTableName(dataSet);

  try
  {
    tableInfo.timestampPropertyName = dataAccessorPostGis->getTimestampPropertyName(dataSet, false);
  }
  catch (...)
  {

  }

  tableInfo.dataSetType = getDataSetType(inputDataProvider->uri,
                                         tableInfo.tableName,
                                         "POSTGIS");

  return tableInfo;
}
