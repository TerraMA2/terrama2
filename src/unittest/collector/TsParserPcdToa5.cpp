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
  \file terrama2/collector/TsParserPcdToa5.cpp

  \brief Tests for the ParserPcdToa5 class.

  \author Jano Simas
*/

#include "TsParserPcdToa5.hpp"
#include "Utils.hpp"

//Qt
#include <QTemporaryDir>
#include <QFile>

//STL
#include <memory>

//Terrama2
#include <terrama2/core/Utils.hpp>
#include <terrama2/core/DataSetItem.hpp>
#include <terrama2/collector/Exception.hpp>
#include <terrama2/collector/DataFilter.hpp>
#include <terrama2/collector/ParserPcdToa5.hpp>

//terralib
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/datatype/TimeInstantTZ.h>

//boost
#include <boost/date_time/gregorian/greg_date.hpp>



void TsParserPcdToa5::TestParseCpvOk()
{

  //FIXME: Review ParserToa5 not working.
  //reading first line as header, second line is the header
  QFAIL("NOT WORKING!!!");

  try
  {
    terrama2::core::DataSetItem item;
    item.setMask("CPV_slow_%A_%M_%d_%h%m.dat");

    terrama2::collector::DataFilterPtr filter = std::make_shared<terrama2::collector::DataFilter>(item);

    terrama2::collector::TransferenceData transferenceData;
    transferenceData.uriTemporary = terrama2::core::FindInTerraMA2Path("data/pcd_toa5/CPV/");

    std::vector<terrama2::collector::TransferenceData> transferenceDataVec;
    transferenceDataVec.push_back(transferenceData);

    terrama2::collector::ParserPcdToa5 parser;
    parser.read(filter, transferenceDataVec);

    QVERIFY(transferenceDataVec.size() == 21);

    std::shared_ptr<te::da::DataSet> dataset = transferenceDataVec.at(0).teDataSet;
    if(dataset->moveNext())
    {
      std::unique_ptr<te::dt::DateTime> dateTime(dataset->getDateTime("DateTime"));
      QVERIFY(dateTime.get());
      te::dt::TimeInstantTZ* timeTz = dynamic_cast<te::dt::TimeInstantTZ*>(dateTime.get());
      QVERIFY(timeTz);

      boost::gregorian::date localDate = timeTz->getTimeInstantTZ().date();
      QVERIFY(localDate.day() == 1);
      QVERIFY(localDate.month() == 11);
      QVERIFY(localDate.year() == 2015);

      boost::posix_time::time_duration time = timeTz->getTimeInstantTZ().utc_time().time_of_day();
      QCOMPARE(time.hours(), 3);
      QCOMPARE(time.minutes(), 0);
      QCOMPARE(time.seconds(), 0);
    }

  }
  catch(...)
  {
    QFAIL(NO_EXCEPTION_EXPECTED);
  }

  return;
}

void TsParserPcdToa5::TestParseFail()
{
  QFAIL("NOT IMPLEMENTED");
}
