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
  \file terrama2/collector/TsFilter.hpp

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

#include <terrama2/core/DataProvider.hpp>
#include <terrama2/core/DataSetItem.hpp>
#include <terrama2/core/DataSet.hpp>
#include <terrama2/core/Filter.hpp>

//QT
#include <QStringList>

//terralib
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/datatype/Date.h>

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

  MockLog collectLog;
  EXPECT_CALL(collectLog, getDataSetItemLastDateTime(::testing::_))
      .Times(1)
      .WillRepeatedly(::testing::Return(nullptr));
  terrama2::collector::DataFilter datafilter(dataItem, collectLog);

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

  MockLog collectLog;
  EXPECT_CALL(collectLog, getDataSetItemLastDateTime(::testing::_))
      .Times(1)
      .WillRepeatedly(::testing::Return(nullptr));
  try
  {
    terrama2::collector::DataFilter datafilter(dataItem, collectLog);

    QFAIL(NO_EXCEPTION_THROWN);
  }
  catch(terrama2::collector::EmptyMaskError& e)
  {
    return;
  }
  catch(...)
  {
    QFAIL(WRONG_TYPE_EXCEPTION);
  }

  QFAIL(UNEXPECTED_BEHAVIOR);
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

  MockLog collectLog;
  EXPECT_CALL(collectLog, getDataSetItemLastDateTime(::testing::_))
      .Times(2)
      .WillRepeatedly(::testing::Return(nullptr));


  //test for dates
  dataItem.setMask("name_%d/%M/%A");
  {
    terrama2::collector::DataFilter datafilter(dataItem, collectLog);

    std::vector<std::string> names {"name_12/03/2015", "name 12/03/2015 ", "name_12/3/2015", "name_12-03-2015", "names_2A/03/2015 "};

    names = datafilter.filterNames(names);

    QVERIFY(names.size() == 1);
  }

  //test for time
  dataItem.setMask("name_%h:%m:%s");
  {
    terrama2::collector::DataFilter datafilter(dataItem, collectLog);

    std::vector<std::string> names {"name_12:03:15", "name 12:03:15 ", "name_12:3:15", "name_12:03:2015", "names_2A:03:2015 "};

    names = datafilter.filterNames(names);

    QVERIFY(names.size() == 1);
  }
}

void TsDataFilter::TestgetDataSetLastDateTime()
{
  terrama2::core::DataProvider provider;
  terrama2::core::DataSet      dataset;
  terrama2::core::DataSetItem  dataItem;
  terrama2::core::Filter       filter;
  boost::local_time::time_zone_ptr zone(new  boost::local_time::posix_time_zone("+00"));
  boost::local_time::local_date_time boostTime(boost::gregorian::date(2015,3,12), boost::posix_time::time_duration(10,35,15,0), zone, true);
  std::unique_ptr<te::dt::TimeInstantTZ> tDate(new te::dt::TimeInstantTZ(boostTime));

  provider.add(dataset);
  dataset.add(dataItem);
  dataItem.setFilter(filter);
  dataItem.setTimezone(zone->to_posix_string());

  std::string exact("name_%d/%M/%A_%h:%m:%s");
  dataItem.setMask(exact);

  MockLog collectLog;
  EXPECT_CALL(collectLog, getDataSetItemLastDateTime(::testing::_))
      .Times(1)
      .WillRepeatedly(::testing::Return(nullptr));
  terrama2::collector::DataFilter datafilter(dataItem, collectLog);
  std::vector<std::string> names {"name_12/03/2015_10:30:15",
                                  "name_12/03/2015_10:31:15",
                                  "name_12/03/2015_10:32:15",
                                  "name_12/03/2015_10:33:15",
                                  "name_12/03/2015_10:35:15",// <<< Max value
                                  "name_12/03/2015_10:34:15", };

  names = datafilter.filterNames(names);

  int size = names.size();
  QCOMPARE(size, 6);

  te::dt::TimeInstantTZ* dataSetLastDateTime = datafilter.getDataSetLastDateTime();

  QVERIFY(dataSetLastDateTime);

  QCOMPARE(*tDate, *dataSetLastDateTime);
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

  MockLog collectLog;
  EXPECT_CALL(collectLog, getDataSetItemLastDateTime(::testing::_))
      .Times(1)
      .WillRepeatedly(::testing::Return(nullptr));
  terrama2::collector::DataFilter datafilter(dataItem, collectLog);

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

  MockLog collectLog;
  EXPECT_CALL(collectLog, getDataSetItemLastDateTime(::testing::_))
      .Times(1)
      .WillRepeatedly(::testing::Return(lastCollectedDate));
  terrama2::collector::DataFilter datafilter(dataItem, collectLog);

  std::vector<std::string> names {"name_12/03/2015", "name_12/07/2014", "name_12/06/2015", "name_16/06/2015", "name_12-11-2015", "name_12/18/2015", };

  names = datafilter.filterNames(names);

  QVERIFY(names.size() == 1);
}

