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
  \file terrama2/core/data-access/DataAccessorPostGis.hpp

  \brief

  \author Jano Simas
 */

#include "DataAccessorPostGis.hpp"
#include "../core/utility/Raii.hpp"

//TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

//QT
#include <QUrl>
#include <QObject>

 std::shared_ptr<te::mem::DataSet> terrama2::core::DataAccessorPostGis::getDataSet(const std::string& uri, const Filter& filter, DataSetPtr dataSet) const
 {
   QUrl url(uri.c_str());

   std::string tableName = getTableName(dataSet);

   // creates a DataSource to the data and filters the dataset,
   // also joins if the DCP comes from separated files
   std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make(dataSourceTye()));

   std::map<std::string, std::string> connInfo{{"PG_HOST", url.host().toStdString()},
                                               {"PG_PORT", std::to_string(url.port())},
                                               {"PG_USER", url.userName().toStdString()},
                                               {"PG_PASSWORD", url.password().toStdString()},
                                               {"PG_DB_NAME", url.path().section("/", 1, 1).toStdString()},
                                               {"PG_CONNECT_TIMEOUT", "4"},
                                               {"PG_CLIENT_ENCODING", "UTF-8"}
                                             };

   datasource->setConnectionInfo(connInfo);

   //RAII for open/closing the datasource
   OpenClose<std::shared_ptr<te::da::DataSource> > openClose(datasource);

   if(!datasource->isOpened())
   {
     QString errMsg = QObject::tr("DataProvider could not be opened.");
     TERRAMA2_LOG_ERROR() << errMsg;
     //TODO: throw here
     throw;
   }

   // get a transactor to interact to the data source
   std::shared_ptr<te::da::DataSourceTransactor> transactor(datasource->getTransactor());
   //TODO: implement filter in query
   std::shared_ptr<te::da::DataSet> teDataSet = transactor->getDataSet(tableName);
   if(teDataSet->isEmpty())
   {
     QString errMsg = QObject::tr("No data in dataset: %1.").arg(dataSet->id);
     TERRAMA2_LOG_ERROR() << errMsg;
     throw NoDataException() << ErrorDescription(errMsg);
   }

   std::shared_ptr<te::mem::DataSet> completeDataset = std::make_shared<te::mem::DataSet>(*teDataSet);

   return completeDataset;
 }

std::string terrama2::core::DataAccessorPostGis::retrieveData(const DataRetrieverPtr dataRetriever, DataSetPtr dataSet, const Filter& filter) const
{
  QString errMsg = QObject::tr("Non retrievable DataProvider.");
  TERRAMA2_LOG_ERROR() << errMsg;
  throw NoDataException() << ErrorDescription(errMsg);
}
