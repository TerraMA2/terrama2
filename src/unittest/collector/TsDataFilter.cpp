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
  \file terrama2/collector/TsFilter.cpp

  \brief Tests for the TestFilter class.

  \author Jano Simas
*/

#include "TsDataFilter.hpp"
#include "Utils.hpp"
#include "Mock.hpp"

//terrama2
#include <terrama2/collector/DataFilter.hpp>
#include <terrama2/collector/Exception.hpp>
#include <terrama2/collector/Utils.hpp>
#include <terrama2/collector/Log.hpp>
#include <terrama2/collector/TransferenceData.hpp>

#include <terrama2/core/DataProvider.hpp>
#include <terrama2/core/DataSetItem.hpp>
#include <terrama2/core/DataSet.hpp>
#include <terrama2/core/Filter.hpp>

//QT
#include <QStringList>
#include <QDebug>

//terralib
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/datatype/Date.h>
#include <terralib/geometry/Polygon.h>
#include <terralib/memory/DataSet.h>
#include <terralib/memory/DataSetItem.h>

//STL
#include <utility>

//boost
#include <boost/date_time/local_time/local_time.hpp>

void TsDataFilter::TestFilterNamesExact()
{
  terrama2::core::DataProvider provider;
  terrama2::core::DataSet      dataset;
  terrama2::core::DataSetItem  dataItem;
  terrama2::core::Filter       filter;
  provider.add(dataset);
  dataset.add(dataItem);
  dataItem.setFilter(filter);

  std::string exact("exact");
  dataItem.setMask(exact);

  terrama2::collector::DataFilter datafilter(dataItem);

  std::vector<std::string> names {"teste1", "teste2 ", "exc", "exact", "exact "};

  names = datafilter.filterNames(names);

  QVERIFY(names.size() == 1);
  QCOMPARE(names.at(0), exact);
}

void TsDataFilter::TestEmptyMask()
{
  terrama2::core::DataProvider provider;
  terrama2::core::DataSet      dataset;
  terrama2::core::DataSetItem  dataItem;
  terrama2::core::Filter       filter;
  provider.add(dataset);
  dataset.add(dataItem);
  dataItem.setFilter(filter);

  try
  {
    terrama2::collector::DataFilter datafilter(dataItem);

    QFAIL(NO_EXCEPTION_THROWN);
  }
  catch(const terrama2::collector::EmptyMaskException& e)
  {
    return;
  }
  catch(...)
  {
    QFAIL(WRONG_TYPE_EXCEPTION);
  }

  QFAIL(UNEXPECTED_BEHAVIOR);
}

void TsDataFilter::TestGeometry()
{
  try {
    terrama2::core::DataProvider provider;
    terrama2::core::DataSet      dataset;
    terrama2::core::DataSetItem  dataItem;
    dataItem.setMask("__DUMMY__");

    provider.add(dataset);
    dataset.add(dataItem);

    terrama2::core::Filter       filter;

    te::gm::LinearRing* s = new te::gm::LinearRing(5, te::gm::LineStringType, 4326);
    const double xc(10), yc(10), halfSize(5);
    s->setPoint(0, xc - halfSize, yc - halfSize); // lower left
    s->setPoint(1, xc - halfSize, yc + halfSize); // upper left
    s->setPoint(2, xc + halfSize, yc + halfSize); // upper rigth
    s->setPoint(3, xc + halfSize, yc - halfSize); // lower rigth
    s->setPoint(4, xc - halfSize, yc - halfSize); // closing

    std::unique_ptr< te::gm::Polygon > geom(new te::gm::Polygon(0, te::gm::PolygonType, 4326));
    geom->push_back(s);
    filter.setGeometry(std::move(geom));
    dataItem.setFilter(filter);

    terrama2::collector::DataFilter datafilter(dataItem);

    te::gm::GeometryProperty* geomProperty = new te::gm::GeometryProperty("GeomProperty");
    std::shared_ptr<te::da::DataSetType> datasetType = std::make_shared<te::da::DataSetType>("DataSetType");
    datasetType->add(geomProperty);

    std::shared_ptr<te::mem::DataSet> dataSet = std::make_shared<te::mem::DataSet>(datasetType.get());
    te::mem::DataSetItem* data = new te::mem::DataSetItem(dataSet.get());
    data->setValue(0, new te::gm::Point(6, 6, 4326, 0));
    dataSet->add(data);

    data = new te::mem::DataSetItem(dataSet.get());
    data->setValue(0, new te::gm::Point(0, 0, 4326, 0));
    dataSet->add(data);

    QVERIFY(dataSet->size() == 2);

    terrama2::collector::TransferenceData transferenceData;
    transferenceData.teDataSet = dataSet;
    transferenceData.teDataSetType = datasetType;

    datafilter.filterDataSet(transferenceData);

    QVERIFY(transferenceData.teDataSet->size() == 1);
  } catch (terrama2::Exception& e) {
    QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());

  } catch (boost::exception& e) {
    QFAIL(boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str());
  } catch (std::exception& e) {
    QFAIL(e.what());
  } catch (...) {
    QFAIL("Unknown error");
  }
}

void TsDataFilter::TestMask()
{
  terrama2::core::DataProvider provider;
  terrama2::core::DataSet      dataset;
  terrama2::core::DataSetItem  dataItem;
  terrama2::core::Filter       filter;
  provider.add(dataset);
  dataset.add(dataItem);
  dataItem.setFilter(filter);

  //test for dates
  dataItem.setMask("name_%d/%M/%A");
  {
    terrama2::collector::DataFilter datafilter(dataItem);

    std::vector<std::string> names {"name_12/03/2015", "name 12/03/2015 ", "name_12/3/2015", "name_12-03-2015", "names_2A/03/2015 "};

    names = datafilter.filterNames(names);

    QVERIFY(names.size() == 1);
  }

  //test for time
  dataItem.setMask("name_%h:%m:%s");
  {
    terrama2::collector::DataFilter datafilter(dataItem);

    std::vector<std::string> names {"name_12:03:15", "name 12:03:15 ", "name_12:3:15", "name_12:03:2015", "names_2A:03:2015 "};

    names = datafilter.filterNames(names);

    QVERIFY(names.size() == 1);
  }
}

void TsDataFilter::TestDiscardBefore()
{
  terrama2::core::DataProvider provider;
  terrama2::core::DataSet      dataset;
  terrama2::core::DataSetItem  dataItem;
  terrama2::core::Filter       filter;
  boost::local_time::time_zone_ptr zone(new  boost::local_time::posix_time_zone("+00"));
  boost::local_time::local_date_time boostTime(boost::gregorian::date(2015,6,15), boost::posix_time::time_duration(0,0,0,0), zone, true);
  std::unique_ptr<te::dt::TimeInstantTZ> tDate(new te::dt::TimeInstantTZ(boostTime));
  filter.setDiscardBefore(std::move(tDate));

  provider.add(dataset);
  dataset.add(dataItem);
  dataItem.setFilter(filter);

  std::string exact("name_%d/%M/%A");
  dataItem.setMask(exact);

  terrama2::collector::DataFilter datafilter(dataItem);

  std::vector<std::string> names {"name_12/03/2015", "name_12/07/2014", "name_12/06/2015", "name_16/06/2015", "name_12-11-2015", "name_12/18/2015", };

  names = datafilter.filterNames(names);

  QVERIFY(names.size() == 1);
}

void TsDataFilter::TestLastCollected()
{
  terrama2::core::DataProvider provider;
  terrama2::core::DataSet      dataset;
  terrama2::core::DataSetItem  dataItem;
  terrama2::core::Filter       filter;
  boost::local_time::time_zone_ptr zone(new  boost::local_time::posix_time_zone("+00"));
  boost::local_time::local_date_time boostTime(boost::gregorian::date(2015,6,15), boost::posix_time::time_duration(0,0,0,0), zone, true);
  std::shared_ptr<te::dt::TimeInstantTZ> lastCollectedDate = std::make_shared<te::dt::TimeInstantTZ>(boostTime);

  provider.add(dataset);
  dataset.add(dataItem);
  dataItem.setFilter(filter);

  std::string exact("name_%d/%M/%A");
  dataItem.setMask(exact);

  terrama2::collector::DataFilter datafilter(dataItem, lastCollectedDate);

  std::vector<std::string> names {"name_12/03/2015", "name_12/07/2014", "name_12/06/2015", "name_16/06/2015", "name_12-11-2015", "name_12/18/2015", };

  names = datafilter.filterNames(names);

  QVERIFY(names.size() == 1);
}

