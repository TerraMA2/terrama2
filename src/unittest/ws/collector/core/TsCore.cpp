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
  \file terrama2/unittest/ws/server/TsCore.cpp

  \brief Tests for the WebProxy class.

  \author Vinicius Campanha
*/

// STL
#include <memory>

// Boost
#include "boost/date_time/posix_time/posix_time.hpp"

// TerraLib
#include <terralib/datatype.h>
#include <terralib/geometry/Polygon.h>

// TerraMA2
#include <terrama2/core/ApplicationController.hpp>
#include <terrama2/ws/collector/core/Utils.hpp>
#include "soapWebService.h"

// TerraMA2 Test
#include "TsCore.hpp"


void TsCore::init()
{
  clearDatabase();
}

void TsCore::cleanup()
{
  clearDatabase();
}

void TsCore::clearDatabase()
{
  std::shared_ptr<te::da::DataSource> dataSource = terrama2::core::ApplicationController::getInstance().getDataSource();

  std::auto_ptr<te::da::DataSourceTransactor> transactor = dataSource->getTransactor();
  transactor->begin();

  std::string query = "TRUNCATE TABLE terrama2.data_provider CASCADE";
  transactor->execute(query);

  transactor->commit();
}

terrama2::core::DataProvider TsCore::buildDataProvider()
{
  terrama2::core::DataProvider  dataProvider("Data Provider", terrama2::core::DataProvider::Kind(1), 0);

  dataProvider.setUri("pathDataProvider");
  dataProvider.setDescription("Data Provider Description");
  dataProvider.setStatus((terrama2::core::DataProvider::Status)1);

  terrama2::core::DataManager::getInstance().add(dataProvider);

  return dataProvider;
}

terrama2::core::DataSet TsCore::buildDataSet()
{
  terrama2::core::DataProvider dataProvider = buildDataProvider();  

  terrama2::core::DataSet dataSet("Data Set Name", (terrama2::core::DataSet::Kind)1, 0, dataProvider.id());

  dataSet.setDescription("Data Set Description");
  dataSet.setStatus((terrama2::core::DataSet::Status)1);

  boost::posix_time::time_duration dataFrequency(boost::posix_time::duration_from_string("00:05:00.00"));
  boost::posix_time::time_duration schedule(boost::posix_time::duration_from_string("00:06:00.00"));
  boost::posix_time::time_duration scheduleRetry(boost::posix_time::duration_from_string("00:07:00.00"));
  boost::posix_time::time_duration scheduleTimeout(boost::posix_time::duration_from_string("00:08:00.00"));

  dataSet.setDataFrequency(te::dt::TimeDuration(dataFrequency));
  dataSet.setSchedule(te::dt::TimeDuration(schedule));
  dataSet.setScheduleRetry(te::dt::TimeDuration(scheduleRetry));
  dataSet.setScheduleTimeout(te::dt::TimeDuration(scheduleTimeout));

  terrama2::core::DataSetItem dataSetItem1(terrama2::core::DataSetItem::Kind(1), 0, 0);
  dataSetItem1.setMask("mask1");
  dataSetItem1.setStatus(terrama2::core::DataSetItem::Status(2));
  dataSetItem1.setTimezone("-1");

  terrama2::core::Filter filter(0);

  te::dt::DateTime* td = new te::dt::TimeInstant(boost::posix_time::ptime(boost::posix_time::time_from_string("2002-01-20 23:59:59.000")));
  std::unique_ptr< te::dt::DateTime > discardBefore(td);
  filter.setDiscardBefore(std::move(discardBefore));

  te::dt::DateTime* td2 = new te::dt::TimeInstant(boost::posix_time::ptime(boost::posix_time::time_from_string("2002-01-21 23:59:59.000")));
  std::unique_ptr< te::dt::DateTime > timeAfter(td2);
  filter.setDiscardAfter(std::move(timeAfter));

  std::unique_ptr< double > value(new double(5.1));
  filter.setValue(std::move(value));

  filter.setExpressionType(terrama2::core::Filter::ExpressionType(1));
  filter.setBandFilter("filter_bandFilter");

  te::gm::LinearRing* s = new te::gm::LinearRing(5, te::gm::LineStringType);

  const double &xc(5), &yc(5), &halfSize(5);
  s->setPoint(0, xc - halfSize, yc - halfSize); // lower left
  s->setPoint(1, xc - halfSize, yc + halfSize); // upper left
  s->setPoint(2, xc + halfSize, yc + halfSize); // upper rigth
  s->setPoint(3, xc + halfSize, yc - halfSize); // lower rigth
  s->setPoint(4, xc - halfSize, yc - halfSize); // closing

  te::gm::Polygon* p = new te::gm::Polygon(0, te::gm::PolygonType);
  p->push_back(s);

  std::unique_ptr< te::gm::Geometry > geom(p);
  filter.setGeometry(std::move(geom));

  dataSetItem1.setFilter(filter);

  std::map< std::string, std::string > storageMetadata;

  storageMetadata["one"] = "two";
  storageMetadata["two"] = "one";

  dataSetItem1.setStorageMetadata(storageMetadata);

  terrama2::core::DataSetItem dataSetItem2(terrama2::core::DataSetItem::Kind(2), 0, 0);
  dataSetItem2.setMask("mask2");
  dataSetItem2.setStatus(terrama2::core::DataSetItem::Status(2));
  dataSetItem2.setTimezone("-2");

  terrama2::core::DataSetItem dataSetItem3(terrama2::core::DataSetItem::Kind(3), 0, 0);
  dataSetItem3.setMask("mask3");
  dataSetItem3.setStatus(terrama2::core::DataSetItem::Status(2));
  dataSetItem3.setTimezone("-3");

  terrama2::core::DataSetItem dataSetItem4(terrama2::core::DataSetItem::Kind(4), 0, 0);
  dataSetItem4.setMask("mask4");
  dataSetItem4.setStatus(terrama2::core::DataSetItem::Status(2));
  dataSetItem4.setTimezone("-4");

  terrama2::core::DataSetItem dataSetItem5(terrama2::core::DataSetItem::Kind(1), 0, 0);
  dataSetItem5.setMask("mask5");
  dataSetItem5.setStatus(terrama2::core::DataSetItem::Status(2));
  dataSetItem5.setTimezone("-5");

  dataSet.add(dataSetItem1);
  dataSet.add(dataSetItem2);
  dataSet.add(dataSetItem3);
  dataSet.add(dataSetItem4);
  dataSet.add(dataSetItem5);

  terrama2::core::DataManager::getInstance().add(dataSet);

  return dataSet;
}

void TsCore::TestConvertDataProviderToDataProviderStruct()
{
  terrama2::core::DataProvider dataProvider = buildDataProvider();

  DataProvider struct_dataProvider = terrama2::ws::collector::core::DataProvider2Struct< DataProvider >(dataProvider);

  QVERIFY2(struct_dataProvider.id == dataProvider.id(), "Id changed after conversion!");
  QVERIFY2(struct_dataProvider.name == dataProvider.name(), "Name changed after conversion!");
  QVERIFY2(struct_dataProvider.kind == dataProvider.kind(), "Kind changed after conversion!");
  QVERIFY2(struct_dataProvider.description == dataProvider.description(), "Description changed after conversion!");
  QVERIFY2(struct_dataProvider.status == dataProvider.status(), "Status changed after conversion!");
  QVERIFY2(struct_dataProvider.uri == dataProvider.uri(), "URI changed after conversion!");
}


void TsCore::TestConvertDataProviderStructToDataProvider()
{
  DataProvider struct_dataProvider;

  struct_dataProvider.id = 7;
  struct_dataProvider.name = "Data Provider";
  struct_dataProvider.kind = 1;
  struct_dataProvider.description = "Data Provider description";
  struct_dataProvider.status = 1;
  struct_dataProvider.uri = "C:/Dataprovider/path";

  terrama2::core::DataProvider dataProvider = terrama2::ws::collector::core::Struct2DataProvider< DataProvider >(struct_dataProvider);

  QVERIFY2(struct_dataProvider.id == dataProvider.id(), "Id changed after conversion!");
  QVERIFY2(struct_dataProvider.name == dataProvider.name(), "Name changed after conversion!");
  QVERIFY2(struct_dataProvider.kind == dataProvider.kind(), "Kind changed after conversion!");
  QVERIFY2(struct_dataProvider.description == dataProvider.description(), "Description changed after conversion!");
  QVERIFY2(struct_dataProvider.status == dataProvider.status(), "Status changed after conversion!");
  QVERIFY2(struct_dataProvider.uri == dataProvider.uri(), "URI changed after conversion!");

}


void TsCore::TestConvertDataSetToDataSetStruct()
{
  terrama2::core::DataSet dataSet = buildDataSet();

  DataSet struct_dataSet = terrama2::ws::collector::core::DataSet2Struct< DataSet, DataSetItem >(dataSet);

  QVERIFY2(dataSet.id() == struct_dataSet.id, "ID changed after conversion!");
  QVERIFY2(dataSet.provider() == struct_dataSet.data_provider_id, "Data Provider changed after conversion!");
  QVERIFY2(dataSet.name() == struct_dataSet.name, "Name changed after conversion!");
  QVERIFY2(dataSet.kind() == struct_dataSet.kind, "Kind changed after conversion!");
  QVERIFY2(dataSet.status() == struct_dataSet.status, "Status changed after conversion!");
  QVERIFY2(dataSet.description() == struct_dataSet.description, "Description changed after conversion!");
  QVERIFY2(dataSet.dataFrequency().toString() == struct_dataSet.data_frequency, "Data Frequency changed after conversion!");
  QVERIFY2(dataSet.schedule().toString() == struct_dataSet.schedule, "Schedule changed after conversion!");
  QVERIFY2(dataSet.scheduleRetry().toString() == struct_dataSet.schedule_retry, "Schedule retry changed after conversion!");
  QVERIFY2(dataSet.scheduleTimeout().toString() == struct_dataSet.schedule_timeout, "Schedule Timeout changed after conversion!");

  for(unsigned int i = 0; i < dataSet.dataSetItems().size(); i++)
  {
    QCOMPARE(dataSet.dataSetItems().at(i).dataset() , struct_dataSet.dataset_items.at(i).dataset);
    QCOMPARE(dataSet.dataSetItems().at(i).id() , struct_dataSet.dataset_items.at(i).id);
    QCOMPARE(dataSet.dataSetItems().at(i).kind(), terrama2::core::DataSetItem::Kind(struct_dataSet.dataset_items.at(i).kind));
    QCOMPARE(dataSet.dataSetItems().at(i).mask() , struct_dataSet.dataset_items.at(i).mask);
    QCOMPARE(dataSet.dataSetItems().at(i).status() , terrama2::core::DataSetItem::Status(struct_dataSet.dataset_items.at(i).status));
    QCOMPARE(dataSet.dataSetItems().at(i).timezone() , struct_dataSet.dataset_items.at(i).timezone);

    QCOMPARE(dataSet.dataSetItems().at(i).filter().datasetItem(), struct_dataSet.dataset_items.at(i).filter_datasetItem);
    QCOMPARE(dataSet.dataSetItems().at(i).filter().expressionType(), terrama2::core::Filter::ExpressionType(struct_dataSet.dataset_items.at(i).filter_expressionType));
    QCOMPARE(dataSet.dataSetItems().at(i).filter().bandFilter(), struct_dataSet.dataset_items.at(i).filter_bandFilter);

    if(dataSet.dataSetItems().at(i).filter().discardBefore() != nullptr)
      QCOMPARE(dataSet.dataSetItems().at(i).filter().discardBefore()->toString() , struct_dataSet.dataset_items.at(i).filter_discardBefore);
    else
      QCOMPARE(struct_dataSet.dataset_items.at(i).filter_discardBefore.c_str(), "");

    if(dataSet.dataSetItems().at(i).filter().discardAfter() != nullptr)
      QCOMPARE(dataSet.dataSetItems().at(i).filter().discardAfter()->toString() , struct_dataSet.dataset_items.at(i).filter_discardAfter);
    else
      QCOMPARE(struct_dataSet.dataset_items.at(i).filter_discardAfter.c_str(), "");

    if(dataSet.dataSetItems().at(i).filter().geometry() != nullptr)
    {
      // VINICIUS: toString() is generating a wrong WKT, need to replace '\n' for ','
      std::string geom = dataSet.dataSetItems().at(i).filter().geometry()->toString();
      std::replace( geom.begin(), geom.end(), '\n', ',');
      QCOMPARE( geom, struct_dataSet.dataset_items.at(i).filter_geometry);
    }
    else
    {
      QCOMPARE(struct_dataSet.dataset_items.at(i).filter_geometry.c_str(), "");
    }

    if(dataSet.dataSetItems().at(i).filter().value() != nullptr)
      QCOMPARE(*dataSet.dataSetItems().at(i).filter().value(), struct_dataSet.dataset_items.at(i).filter_value);
    else
      QVERIFY(std::isnan(struct_dataSet.dataset_items.at(i).filter_value));

    std::map< std::string, std::string > storageMetadata(dataSet.dataSetItems().at(i).storageMetadata());

    int j = 0;
    for(auto& x: storageMetadata)
    {
      QCOMPARE(struct_dataSet.dataset_items.at(i).storageMetadata_keys.at(j), x.first);
      QCOMPARE(struct_dataSet.dataset_items.at(i).storageMetadata_values.at(j), x.second);

      j++;
    }
  }
}


void TsCore::TestConvertDataSetStructToDataSet()
{
  DataSet struct_dataSet;

  struct_dataSet.id = 1;
  struct_dataSet.name = "Data Set";
  struct_dataSet.kind = 1;
  struct_dataSet.status = 1;
  struct_dataSet.description = "Data Set description";
  struct_dataSet.data_frequency = "00:01:00";
  struct_dataSet.schedule = "00:02:00";
  struct_dataSet.schedule_retry = "00:03:00";
  struct_dataSet.schedule_timeout = "00:04:00";
  struct_dataSet.data_provider_id = 1;

  for(int i = 1; i < 5; i++)
  {
    DataSetItem dataset_item;

    dataset_item.kind = i;
    dataset_item.id = i;
    dataset_item.dataset = struct_dataSet.id;
    dataset_item.status = 2;
    dataset_item.mask = "mask" + i;
    dataset_item.timezone = "-3";

    if(i == 2)
    {
      dataset_item.filter_datasetItem = dataset_item.id;
      dataset_item.filter_discardBefore = "2002-Jan-20 23:59:59";
      dataset_item.filter_discardAfter = "2002-Jan-21 23:59:59";

      te::gm::LinearRing* s = new te::gm::LinearRing(5, te::gm::LineStringType);

      const double &xc(5), &yc(5), &halfSize(5);
      s->setPoint(0, xc - halfSize, yc - halfSize); // lower left
      s->setPoint(1, xc - halfSize, yc + halfSize); // upper left
      s->setPoint(2, xc + halfSize, yc + halfSize); // upper rigth
      s->setPoint(3, xc + halfSize, yc - halfSize); // lower rigth
      s->setPoint(4, xc - halfSize, yc - halfSize); // closing

      te::gm::Polygon* p = new te::gm::Polygon(0, te::gm::PolygonType);
      p->push_back(s);

      // VINICIUS: toString() is generating a wrong WKT, need to replace '\n' for ','
      std::string geom = p->toString();
      std::replace( geom.begin(), geom.end(), '\n', ',');
      dataset_item.filter_geometry = geom;
      dataset_item.filter_value = 10;
      dataset_item.filter_expressionType = 2;
      dataset_item.filter_bandFilter = "filter_bandFilter";
    }

    if(i == 3)
      dataset_item.filter_value = std::nan("");

    dataset_item.storageMetadata_keys.push_back("one");
    dataset_item.storageMetadata_values.push_back("two");
    dataset_item.storageMetadata_keys.push_back("two");
    dataset_item.storageMetadata_values.push_back("one");

    struct_dataSet.dataset_items.push_back(dataset_item);
  }

  terrama2::core::DataSet dataSet = terrama2::ws::collector::core::Struct2DataSet< DataSet , DataSetItem >(struct_dataSet);

  QVERIFY2(dataSet.id() == struct_dataSet.id, "ID changed after conversion!");
  QVERIFY2(dataSet.provider() == struct_dataSet.data_provider_id, "Data Provider changed after conversion!");
  QVERIFY2(dataSet.name() == struct_dataSet.name, "Name changed after conversion!");
  QVERIFY2(dataSet.kind() == struct_dataSet.kind, "Kind changed after conversion!");
  QVERIFY2(dataSet.status() == struct_dataSet.status, "Status changed after conversion!");
  QVERIFY2(dataSet.description() == struct_dataSet.description, "Description changed after conversion!");
  QVERIFY2(dataSet.dataFrequency().toString() == struct_dataSet.data_frequency, "Data Frequency changed after conversion!");
  QVERIFY2(dataSet.schedule().toString() == struct_dataSet.schedule, "Schedule changed after conversion!");
  QVERIFY2(dataSet.scheduleRetry().toString() == struct_dataSet.schedule_retry, "Schedule retry changed after conversion!");
  QVERIFY2(dataSet.scheduleTimeout().toString() == struct_dataSet.schedule_timeout, "Schedule Timeout changed after conversion!");

  for(unsigned int i = 0; i < struct_dataSet.dataset_items.size(); i++)
  {
    QCOMPARE(dataSet.dataSetItems().at(i).dataset() , struct_dataSet.dataset_items.at(i).dataset);
    QCOMPARE(dataSet.dataSetItems().at(i).id() , struct_dataSet.dataset_items.at(i).id);
    QCOMPARE(dataSet.dataSetItems().at(i).kind(), terrama2::core::DataSetItem::Kind(struct_dataSet.dataset_items.at(i).kind));
    QCOMPARE(dataSet.dataSetItems().at(i).mask() , struct_dataSet.dataset_items.at(i).mask);
    QCOMPARE(dataSet.dataSetItems().at(i).status() , terrama2::core::DataSetItem::Status(struct_dataSet.dataset_items.at(i).status));
    QCOMPARE(dataSet.dataSetItems().at(i).timezone() , struct_dataSet.dataset_items.at(i).timezone);

    QCOMPARE(dataSet.dataSetItems().at(i).filter().datasetItem(), struct_dataSet.dataset_items.at(i).filter_datasetItem);

    if(struct_dataSet.dataset_items.at(i).filter_expressionType == 0)
      QCOMPARE(dataSet.dataSetItems().at(i).filter().expressionType(), terrama2::core::Filter::ExpressionType::NONE_TYPE);
    else
      QCOMPARE(dataSet.dataSetItems().at(i).filter().expressionType(), terrama2::core::Filter::ExpressionType(struct_dataSet.dataset_items.at(i).filter_expressionType));

    QCOMPARE(dataSet.dataSetItems().at(i).filter().bandFilter(), struct_dataSet.dataset_items.at(i).filter_bandFilter);

    if(!struct_dataSet.dataset_items.at(i).filter_discardBefore.empty())
    {
      QVERIFY(dataSet.dataSetItems().at(i).filter().discardBefore() != nullptr);
      QCOMPARE(dataSet.dataSetItems().at(i).filter().discardBefore()->toString() , struct_dataSet.dataset_items.at(i).filter_discardBefore);
    }
    else
      QVERIFY(dataSet.dataSetItems().at(i).filter().discardBefore() == nullptr);

    if(!struct_dataSet.dataset_items.at(i).filter_discardAfter.empty())
    {
      QVERIFY(dataSet.dataSetItems().at(i).filter().discardAfter() != nullptr);
      QCOMPARE(dataSet.dataSetItems().at(i).filter().discardAfter()->toString() , struct_dataSet.dataset_items.at(i).filter_discardAfter);
    }
    else
      QVERIFY(dataSet.dataSetItems().at(i).filter().discardAfter() == nullptr);

    if(!struct_dataSet.dataset_items.at(i).filter_geometry.empty())
    {
      QVERIFY(dataSet.dataSetItems().at(i).filter().geometry() != nullptr);
      // VINICIUS: toString() is generating a wrong WKT, need to replace '\n' for ','
      std::string geom = dataSet.dataSetItems().at(i).filter().geometry()->toString();
      std::replace( geom.begin(), geom.end(), '\n', ',');
      QCOMPARE( geom, struct_dataSet.dataset_items.at(i).filter_geometry);
    }
    else
    {
      QVERIFY(dataSet.dataSetItems().at(i).filter().geometry() == nullptr);
    }

    if(!std::isnan(struct_dataSet.dataset_items.at(i).filter_value))
    {
      QVERIFY(dataSet.dataSetItems().at(i).filter().value() != nullptr);
      QCOMPARE(*dataSet.dataSetItems().at(i).filter().value(), struct_dataSet.dataset_items.at(i).filter_value);
    }
    else
      QVERIFY(dataSet.dataSetItems().at(i).filter().value() == nullptr);

    std::map< std::string, std::string > storageMetadata(dataSet.dataSetItems().at(i).storageMetadata());

    int j = 0;
    for(auto& x: storageMetadata)
    {
      QCOMPARE(struct_dataSet.dataset_items.at(i).storageMetadata_keys.at(j), x.first);
      QCOMPARE(struct_dataSet.dataset_items.at(i).storageMetadata_values.at(j), x.second);

      j++;
    }
  }
}
