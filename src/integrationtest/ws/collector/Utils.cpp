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
  \file integrationtest/ws/collector/Utils.cpp

  \brief Utility functions to initialize e finalize terralib and TerraMA2 for tests.

  \author Vinicius Campanha
*/


//TerrraMA2
#include "Utils.hpp"
#include <terrama2/core/ApplicationController.hpp>
#include <terrama2/core/DataManager.hpp>
#include <terrama2/core/Utils.hpp>

// TerraLib
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/common/PlatformUtils.h>
#include <terralib/common.h>
#include <terralib/plugin.h>

// QT
#include <QTest>
#include <QJsonDocument>

// STL
#include <string>

void InitializeTerraMA2(std::string project_path)
{
  terrama2::core::initializeTerralib();

  std::string path = terrama2::core::FindInTerraMA2Path(project_path);
  QJsonDocument jdoc = terrama2::core::ReadJsonFile(path);
  QJsonObject project = jdoc.object();
  QCOMPARE(terrama2::core::ApplicationController::getInstance().loadProject(project), true);
  std::shared_ptr<te::da::DataSource> dataSource = terrama2::core::ApplicationController::getInstance().getDataSource();
  QVERIFY(dataSource.get());
  terrama2::core::DataManager::getInstance().load();
}

void FinalizeTerraMA2()
{
  terrama2::core::DataManager::getInstance().unload();

  terrama2::core::finalizeTerralib();
}

terrama2::core::DataProvider buildDataProvider()
{

  terrama2::core::DataProvider  dataProvider("Data Provider", terrama2::core::DataProvider::Kind::FILE_TYPE, 0);

  dataProvider.setUri("file:///../../../../");
  dataProvider.setDescription("Data Provider Description");
  dataProvider.setStatus(terrama2::core::DataProvider::Status::ACTIVE);
  dataProvider.setOrigin(terrama2::core::DataProvider::Origin::COLLECTOR);

  return dataProvider;
}


terrama2::core::DataSet buildDataSet(uint64_t dataProvider_id)
{
  terrama2::core::DataSet dataSet("Data Set Name", terrama2::core::DataSet::Kind::OCCURENCE_TYPE, 0, dataProvider_id);

  dataSet.setDescription("Data Set Description");
  dataSet.setStatus(terrama2::core::DataSet::Status::ACTIVE);

  boost::posix_time::time_duration dataFrequency(boost::posix_time::duration_from_string("00:05:00.00"));
  boost::posix_time::time_duration schedule(boost::posix_time::duration_from_string("00:06:00.00"));
  boost::posix_time::time_duration scheduleRetry(boost::posix_time::duration_from_string("00:07:00.00"));
  boost::posix_time::time_duration scheduleTimeout(boost::posix_time::duration_from_string("00:08:00.00"));

  dataSet.setDataFrequency(te::dt::TimeDuration(dataFrequency));
  dataSet.setSchedule(te::dt::TimeDuration(schedule));
  dataSet.setScheduleRetry(te::dt::TimeDuration(scheduleRetry));
  dataSet.setScheduleTimeout(te::dt::TimeDuration(scheduleTimeout));

//  terrama2::core::Intersection intersection;
//  std::map<std::string, std::vector<std::string> > attrMap;
//  std::vector<std::string> attrVec;
//  attrVec.push_back("geocodigo");
//  attrMap["public.municipio"] = attrVec;
//  intersection.setAttributeMap(attrMap);

//  dataSet.setIntersection(intersection);

  std::map<std::string, std::string> dataset_metadata;

  dataset_metadata["metadataKey"] = "metadataValue";
  dataset_metadata["metadata_Key"] = "metadata_Value";

  dataSet.setMetadata(dataset_metadata);

  terrama2::core::DataSetItem dataSetItem(terrama2::core::DataSetItem::Kind::FIRE_POINTS_TYPE, 0, 0);
  dataSetItem.setMask("fires.csv");
  dataSetItem.setStatus(terrama2::core::DataSetItem::Status::ACTIVE);
  dataSetItem.setTimezone("+00");
  dataSetItem.setPath("data/fire_system/");
  dataSetItem.setSrid(4326);

  terrama2::core::Filter filter(0);

  {
    boost::posix_time::ptime pt(boost::posix_time::time_from_string("2002-01-20 23:59:59.000"));
    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone("MST-07"));
    boost::local_time::local_date_time time(pt, zone);

    std::unique_ptr< te::dt::TimeInstantTZ > discardBefore(new te::dt::TimeInstantTZ(time));
    filter.setDiscardBefore(std::move(discardBefore));
  }

  {
    boost::posix_time::ptime pt(boost::posix_time::time_from_string("2002-01-21 23:59:59.000"));
    boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone("MST-07"));
    boost::local_time::local_date_time time(pt, zone);

    std::unique_ptr< te::dt::TimeInstantTZ > discardAfter(new te::dt::TimeInstantTZ(time));
    filter.setDiscardAfter(std::move(discardAfter));
  }

  std::unique_ptr< double > value(new double(5.1));
  filter.setValue(std::move(value));

  filter.setExpressionType(terrama2::core::Filter::ExpressionType(1));
  filter.setBandFilter("filter_bandFilter");

  te::gm::LinearRing* s = new te::gm::LinearRing(5, te::gm::LineStringType, 4326);

  const double &xc(5), &yc(5), &halfSize(5);
  s->setPoint(0, xc - halfSize, yc - halfSize); // lower left
  s->setPoint(1, xc - halfSize, yc + halfSize); // upper left
  s->setPoint(2, xc + halfSize, yc + halfSize); // upper rigth
  s->setPoint(3, xc + halfSize, yc - halfSize); // lower rigth
  s->setPoint(4, xc - halfSize, yc - halfSize); // closing

  te::gm::Polygon* p = new te::gm::Polygon(0, te::gm::PolygonType, 4326);
  p->push_back(s);

  std::unique_ptr< te::gm::Polygon > geom(p);
  filter.setGeometry(std::move(geom));

  dataSetItem.setFilter(filter);

  std::map< std::string, std::string > metadata;

  metadata["KIND"] = "POSTGIS";
  metadata["PG_CLIENT_ENCODING"] = "UTF-8";
  metadata["PG_DB_NAME"] = "terrama2_test_ws";
  metadata["PG_HOST"] = "localhost";
  metadata["PG_PASSWORD"] = "postgres";
  metadata["PG_PORT"] = "5432";
  metadata["PG_USER"] = "postgres";

  dataSetItem.setMetadata(metadata);

  dataSet.add(dataSetItem);

  return dataSet;
}
