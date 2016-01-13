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
  \file terrama2/collector/TsParserPcdInpe.cpp

  \brief Tests for the ParserPcdInpe class.

  \author Jano Simas
*/

#include "TsParserPcdInpe.hpp"
#include "Utils.hpp"

//Qt
#include <QTemporaryDir>
#include <QFile>
#include <QUrl>

//STL
#include <memory>

//Terrama2
#include <terrama2/core/DataSetItem.hpp>
#include <terrama2/collector/Exception.hpp>
#include <terrama2/collector/DataFilter.hpp>
#include <terrama2/collector/ParserPcdInpe.hpp>
#include <terrama2/collector/TransferenceData.hpp>

//terralib
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/datatype/TimeInstantTZ.h>

//boost
#include <boost/date_time/gregorian/greg_date.hpp>



void TsParserPcdInpe::TestParseOk()
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
  QFile qfile(dir.path()+"/30885.txt");
  if(qfile.exists())
    qfile.remove();

  RAIIQFile raii(qfile); Q_UNUSED(raii);

  // column names:
  // N/A for date
  // <DCPnumber>.<columnName>

  qfile.write("N/A,30885.Bateria,30885.CorrPSol,30885.DirVelVentoMax,30885.DirVento,30885.Pluvio,30885.PressaoAtm,30885.RadSolAcum,30885.TempAr,30885.TempMax,30885.TempMin,30885.TempSolo10,30885.TempSolo20,30885.TempSolo50,30885.UmidInt,30885.UmidRel,30885.UmidSolo10,30885.UmidSolo20,30885.UmidSolo50,30885.VelVento,30885.VelVentoMax\n");
  qfile.write("11/01/2015 03:00:00,12.600,0.000,170.000,170.000,0.000,902.000,0.000,19.500,31.000,15.500,35.500,35.500,35.500,15.000,72.000,0.420,0.080,0.600,5.000,10.000\n");
  qfile.write("11/01/2015 06:00:00,12.600,0.000,180.000,170.000,0.000,901.200,0.000,19.000,31.000,15.500,35.500,35.500,35.500,15.000,74.000,0.340,0.020,0.600,2.900,7.600\n");
  qfile.write("11/01/2015 09:00:00,12.500,0.000,180.000,200.000,0.000,902.200,0.000,19.000,31.000,15.500,35.500,35.500,35.500,15.000,76.000,0.280,1.240,0.580,0.400,6.500\n");
  qfile.write("11/01/2015 12:00:00,13.200,1.000,0.000,10.000,0.000,903.400,1.900,22.000,31.000,18.500,35.500,35.500,35.500,15.000,64.000,0.300,1.160,0.560,1.300,6.900\n");
  qfile.write("11/01/2015 15:00:00,13.000,1.000,50.000,50.000,0.000,903.200,4.500,22.500,31.000,18.500,35.500,35.500,35.500,25.000,76.000,0.460,1.140,0.560,1.100,5.300\n");
  qfile.write("11/01/2015 18:00:00,12.800,1.000,140.000,130.000,3.200,901.700,2.300,19.500,31.000,18.500,35.500,35.500,35.500,25.000,96.000,0.440,1.240,0.420,3.100,6.100\n");
  qfile.write("11/01/2015 21:00:00,12.700,0.000,150.000,140.000,6.200,901.500,0.700,18.500,31.000,18.500,35.500,35.500,35.500,15.000,90.000,0.620,0.160,0.460,4.000,9.000\n");
  qfile.write("11/02/2015 00:00:00,12.600,0.000,120.000,150.000,7.400,903.000,0.000,18.000,25.000,17.500,35.500,35.500,35.500,15.000,92.000,0.680,0.200,0.540,2.500,7.000\n");
  qfile.write("11/02/2015 03:00:00,12.500,0.000,80.000,30.000,11.000,903.200,0.000,17.500,25.000,17.500,35.500,35.500,35.500,15.000,96.000,0.960,0.200,0.580,1.500,5.100");
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

    terrama2::collector::ParserPcdInpe parser;
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

void TsParserPcdInpe::TestParseFail()
{
  QFAIL("NOT IMPLEMENTED");
}
