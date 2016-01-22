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
  \file unittest/core/TsSerializer.cpp

  \brief Test for Serializer class

  \author Paulo R. M. Oliveira
*/

//TerraMA2
#include "TsSerializer.hpp"
#include <terrama2/core/Intersection.hpp>

// TerraLib
#include <terralib/geometry/Geometry.h>
#include <terralib/geometry/LinearRing.h>
#include <terralib/geometry/Polygon.h>
#include <terralib/datatype/TimeInstantTZ.h>

// Qt
#include <QJsonObject>

//boost
#include <boost/date_time/local_time/local_time.hpp>

using namespace terrama2::core;

terrama2::core::Intersection TsSerializer::createIntersection()
{
  terrama2::core::Intersection intersection(1);

  std::vector<std::string> attrList;
  attrList.push_back("attr1");
  attrList.push_back("attr2");
  attrList.push_back("attr3");
  std::map<std::string, std::vector<std::string> > attributeMap;
  attributeMap["dataset1"] = attrList;
  attributeMap["dataset2"] = attrList;

  std::map<uint64_t, std::string> bandMap;
  bandMap[2] = "attr1";
  bandMap[3] = "attr1";

  intersection.setAttributeMap(attributeMap);
  intersection.setBandMap(bandMap);

  return intersection;
}

terrama2::core::Filter TsSerializer::createFilter()
{
  terrama2::core::Filter filter(1);
  filter.setBandFilter("1-2");
  filter.setExpressionType(terrama2::core::Filter::GREATER_THAN_TYPE);

  std::unique_ptr<double> value(new double(10));
  filter.setValue(std::move(value));

  te::gm::LinearRing* s = new te::gm::LinearRing(5, te::gm::LineStringType);

  double xc(5), yc(5), halfSize(5);
  s->setPoint(0, xc - halfSize, yc - halfSize); // lower left
  s->setPoint(1, xc - halfSize, yc + halfSize); // upper left
  s->setPoint(2, xc + halfSize, yc + halfSize); // upper rigth
  s->setPoint(3, xc + halfSize, yc - halfSize); // lower rigth
  s->setPoint(4, xc - halfSize, yc - halfSize); // closing

  te::gm::Polygon* p = new te::gm::Polygon(0, te::gm::PolygonType);
  p->push_back(s);

  std::unique_ptr< te::gm::Polygon > geom(p);
  filter.setGeometry(std::move(geom));

  boost::local_time::time_zone_ptr zone(new  boost::local_time::posix_time_zone("+00"));
  boost::local_time::local_date_time boostTime(boost::gregorian::date(2015,6,15), boost::posix_time::time_duration(0,0,0,0), zone, true);
  std::unique_ptr<te::dt::TimeInstantTZ> tDate(new te::dt::TimeInstantTZ(boostTime));
  filter.setDiscardBefore(std::move(tDate));

  boost::local_time::time_zone_ptr zoneAfter(new  boost::local_time::posix_time_zone("+00"));
  boost::local_time::local_date_time boostTimeAfter(boost::gregorian::date(2015,6,20), boost::posix_time::time_duration(0,0,0,0), zoneAfter, true);
  std::unique_ptr<te::dt::TimeInstantTZ> tDateAfter(new te::dt::TimeInstantTZ(boostTimeAfter));
  filter.setDiscardAfter(std::move(tDateAfter));

  return filter;
}

terrama2::core::DataSetItem TsSerializer::createDataSetItem()
{
  DataSetItem dataSetItem(DataSetItem::PCD_INPE_TYPE, 0, 1);

  dataSetItem.setPath("path");

  Filter filter(dataSetItem.id());
  filter.setExpressionType(Filter::GREATER_THAN_TYPE);
  filter.setValue(std::move(std::unique_ptr<double>(new double(100.))));
  dataSetItem.setFilter(filter);
  dataSetItem.setSrid(4326);
  return dataSetItem;
}

terrama2::core::DataSet TsSerializer::createDataSet()
{
  DataSet dataSet("Queimadas", DataSet::GRID_TYPE, 2, 1);
  dataSet.setStatus(DataSet::Status::ACTIVE);

  te::dt::TimeDuration dataFrequency(2,0,0);
  dataSet.setDataFrequency(dataFrequency);

  std::map<std::string, std::string> metadata;
  metadata["key"] = "value";
  metadata["key1"] = "value1";
  metadata["key2"] = "value2";

  dataSet.setMetadata(metadata);

  Intersection intersection = createIntersection();
  dataSet.setIntersection(intersection);


  DataSetItem dataSetItem = createDataSetItem();
  dataSet.add(dataSetItem);
  dataSet.add(dataSetItem);
  return dataSet;
}

terrama2::core::DataProvider TsSerializer::createDataProvider()
{
  auto dataProvider = terrama2::core::DataProvider("Server 1", terrama2::core::DataProvider::FTP_TYPE, 1);
  dataProvider.setOrigin(terrama2::core::DataProvider::COLLECTOR);
  dataProvider.setStatus(terrama2::core::DataProvider::ACTIVE);
  dataProvider.setDescription("This server...");
  dataProvider.setUri("localhost@...");

  auto dataset = createDataSet();
  dataProvider.add(dataset);
  dataProvider.add(dataset);
  return dataProvider;
}


void TsSerializer::testIntersection()
{
  terrama2::core::Intersection intersection = createIntersection();
  auto jsonObejct = intersection.toJson();
  terrama2::core::Intersection intersection2 = terrama2::core::Intersection::FromJson(jsonObejct);

  QVERIFY2(intersection == intersection2, "Intersection is different");

}

void TsSerializer::testFilter()
{
  terrama2::core::Filter filter = createFilter();

  auto json = filter.toJson();
  terrama2::core::Filter filter2 = terrama2::core::Filter::FromJson(json);

  QVERIFY2(filter == filter2, "Filter is different");
}


void TsSerializer::testDataSetItem()
{
  auto item = createDataSetItem();
  auto json = item.toJson();
  auto item2 = terrama2::core::DataSetItem::FromJson(json);

  QVERIFY2(item == item2, "Item is different");
}

void TsSerializer::testDataSet()
{
  auto dataset = createDataSet();
  auto json = dataset.toJson();
  auto dataset2 = terrama2::core::DataSet::FromJson(json);

  QVERIFY2(dataset == dataset2, "Dataset is different");
}

void TsSerializer::testDataProvider()
{
  auto dataProvider = createDataProvider();
  auto json = dataProvider.toJson();
  auto dataProvider2 = terrama2::core::DataProvider::FromJson(json);

  QVERIFY2(dataProvider == dataProvider2, "Data Provider is different");

}

