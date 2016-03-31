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

#include <terralib/dataaccess/query/LiteralDateTime.h>
#include <terralib/dataaccess/query/ST_Intersects.h>
#include <terralib/dataaccess/query/PropertyName.h>
#include <terralib/dataaccess/query/DataSetName.h>
#include <terralib/dataaccess/query/GreaterThan.h>
#include <terralib/dataaccess/query/LiteralGeom.h>
#include <terralib/dataaccess/query/LessThan.h>
#include <terralib/dataaccess/query/Fields.h>
#include <terralib/dataaccess/query/Select.h>
#include <terralib/dataaccess/query/Field.h>
#include <terralib/dataaccess/query/Where.h>
#include <terralib/dataaccess/query/From.h>
#include <terralib/dataaccess/query/And.h>

#include <terralib/geometry/MultiPolygon.h>

//QT
#include <QUrl>
#include <QObject>

void terrama2::core::DataAccessorPostGis::getDataSet(const std::string& uri,
                                                     const terrama2::core::Filter& filter, terrama2::core::DataSetPtr dataSet,
                                                     std::shared_ptr<te::mem::DataSet>& teDataSet,
                                                     std::shared_ptr<te::da::DataSetType>& teDataSetType) const
{
 QUrl url(uri.c_str());

 std::string tableName = getTableName(dataSet);

 // creates a DataSource to the data and filters the dataset,
 // also joins if the DCP comes from separated files
 std::shared_ptr<te::da::DataSource> datasource(te::da::DataSourceFactory::make(dataSourceType()));

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

 te::da::PropertyName* dateTimeProperty = new te::da::PropertyName(getDateTimeColumnName(dataSet));
 te::da::PropertyName* geometryProperty = new te::da::PropertyName(getGeometryColumnName(dataSet));

 std::vector<te::da::Expression*> where;
 if(filter.discardBefore.get())
 {
   te::da::Expression* discardBeforeVal = new te::da::LiteralDateTime(dynamic_cast<te::dt::DateTime*>(filter.discardBefore->clone()));
   te::da::Expression* discardBeforeExpression = new te::da::GreaterThan(dateTimeProperty->clone(), discardBeforeVal);

   where.push_back(discardBeforeExpression);
 }

 if(filter.discardAfter.get())
 {
   te::da::Expression* discardAfterVal = new te::da::LiteralDateTime(dynamic_cast<te::dt::DateTime*>(filter.discardAfter->clone()));
   te::da::Expression* discardAfterExpression = new te::da::LessThan(dateTimeProperty->clone(), discardAfterVal);

   where.push_back(discardAfterExpression);
 }

 if(filter.geometry.get())
 {
   te::da::Expression* geometryVal = new te::da::LiteralGeom(dynamic_cast<te::gm::Geometry*>(filter.geometry->clone()));
   te::da::Expression* intersectExpression = new te::da::ST_Intersects(geometryProperty, geometryVal);

   where.push_back(intersectExpression);
 }

 te::da::FromItem* t1 = new te::da::DataSetName(tableName);
 te::da::From* from = new te::da::From;
 from->push_back(t1);

 te::da::Where* whereCondition = nullptr;
 if(!where.empty())
 {

   te::da::Expression* expr = where.front();
   for(int i = 1; i < where.size(); ++i)
     expr = new te::da::And(expr, where.at(i));

   whereCondition = new te::da::Where(expr);
 }

 te::da::Fields* fields = new te::da::Fields;
 te::da::PropertyName* pName = new te::da::PropertyName("*");
 te::da::Field* propertyName = new te::da::Field(pName);
 fields->push_back(propertyName);

 te::da::Select select(fields, from, whereCondition);
 std::shared_ptr<te::da::DataSet> tempDataSet = transactor->query(select);
 if(tempDataSet->isEmpty())
 {
   QString errMsg = QObject::tr("No data in dataset: %1.").arg(dataSet->id);
   TERRAMA2_LOG_ERROR() << errMsg;
   throw NoDataException() << ErrorDescription(errMsg);
 }

 std::shared_ptr<te::mem::DataSet> completeDataset = std::make_shared<te::mem::DataSet>(*tempDataSet);

 teDataSet = completeDataset;
 teDataSetType = transactor->getDataSetType(tableName);
}

std::string terrama2::core::DataAccessorPostGis::retrieveData(const DataRetrieverPtr dataRetriever, DataSetPtr dataSet, const Filter& filter) const
{
  QString errMsg = QObject::tr("Non retrievable DataProvider.");
  TERRAMA2_LOG_ERROR() << errMsg;
  throw NoDataException() << ErrorDescription(errMsg);
}
