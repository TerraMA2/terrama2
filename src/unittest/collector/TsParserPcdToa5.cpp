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
  class RAIIQFile
  {
  public:
    RAIIQFile(QFile& file)
      : file_(file)
    {
      file.open(QIODevice::ReadWrite);
    }

    ~RAIIQFile()
    {
      file_.remove();
    }

    QFile& file_;
  };

  QTemporaryDir dir;
  //always <DCPnumber>.txt
  QFile qfile(dir.path()+"/CPV_slow_2014_01_02_1931.dat");
  if(qfile.exists())
    qfile.remove();

  RAIIQFile raii(qfile); Q_UNUSED(raii);

  // column names:
  qfile.write("\"TOA5\",\"CPV\",\"CR1000\",\"20186\",\"CR1000.Std.26\",\"CPU:FIELD_MILL_EFM550_VAISALA_CPV_V1.CR1\",\"10090\",\"slow\" \n");
  qfile.write("\"TIMESTAMP\",\"RECORD\",\"E_field_Avg\",\"leakage_cur_Avg\",\"panel_temp_Avg\",\"battery_volt_Avg\",\"internal_RH_Avg\" \n");
  qfile.write("\"TS\",\"RN\",\"volts/m\",\"nA\",\"DegC\",\"volt\",\"%\"\n");
  qfile.write("\"\",\"\",\"Avg\",\"Avg\",\"Avg\",\"Avg\",\"Avg\"\n");
  qfile.write("\"2015-01-02 19:31:00\",20173,47.68479,\"NAN\",32.4,13.19,39.96\n");
  qfile.write("\"2015-01-02 19:31:00\",20173,47.68479,\"NAN\",32.4,13.19,39.96\n");
  qfile.write("\"2015-01-02 19:32:00\",20174,154.0583,\"NAN\",32.37,13.19,39.96\n");
  qfile.close();
  QFileInfo info(qfile);

  QUrl uri;
  uri.setScheme("file");
  uri.setPath(info.absoluteFilePath());

  QVERIFY(info.exists());

  try
  {
    terrama2::collector::TransferenceData transferenceData;
    transferenceData.uri_temporary = uri.url().toStdString();

    std::vector<terrama2::collector::TransferenceData> transferenceDataVec;
    transferenceDataVec.push_back(transferenceData);


    terrama2::core::DataSetItem item;
    item.setMask(info.fileName().toStdString());

    terrama2::collector::DataFilterPtr filter = std::make_shared<terrama2::collector::DataFilter>(item);

    terrama2::collector::ParserPcdToa5 parser;
    parser.read(filter, transferenceDataVec);

    QVERIFY(transferenceDataVec.size() == 1);

    std::shared_ptr<te::da::DataSet> dataset = transferenceDataVec.at(0).teDataset;
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
