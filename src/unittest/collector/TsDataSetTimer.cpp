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
  \file terrama2/collector/TsDataSetTimer.cpp

  \brief Tests for the DataSetTimer class.

  \author Jano Simas
*/

#include "TsDataSetTimer.hpp"
#include "Utils.hpp"

#include <terrama2/collector/DataSetTimer.hpp>
#include <terrama2/collector/Exception.hpp>
#include <terrama2/core/DataSet.hpp>

//Qt
#include <QtTest/QTest>
#include <QMetaType>//for signals with uint32_t

//STD
#include <cstdint>

void TsDataSetTimer::TestNullDataSet()
{
  try
  {
    //no id
    terrama2::core::DataSet nullDataSet("dummy", terrama2::core::DataSet::UNKNOWN_TYPE);

    terrama2::collector::DataSetTimer nullDataSetTimer(nullDataSet);

    QFAIL(UNEXPECTED_BEHAVIOR);
  }
  catch(terrama2::collector::InvalidDataSetException& e)
  {
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(WRONG_TYPE_EXCEPTION);
  }
  catch(...)
  {
    QFAIL(WRONG_TYPE_EXCEPTION);
  }

  try
  {
    //no name
    terrama2::core::DataSet nullDataSet("", terrama2::core::DataSet::UNKNOWN_TYPE, 1);

    terrama2::collector::DataSetTimer nullDataSetTimer(nullDataSet);

    QFAIL(UNEXPECTED_BEHAVIOR);
  }
  catch(terrama2::collector::InvalidDataSetException& e)
  {
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(WRONG_TYPE_EXCEPTION);
  }
  catch(...)
  {
    QFAIL(WRONG_TYPE_EXCEPTION);
  }

}

void TsDataSetTimer::TestTimerSignalEmit()
{
  try
  {
    terrama2::core::DataSet dataSet("dummy", terrama2::core::DataSet::PCD_TYPE, 1);
    te::dt::TimeDuration freq(0,0,5);
    dataSet.setDataFrequency(freq);
    terrama2::collector::DataSetTimer dataSetTimer(dataSet);

    qRegisterMetaType<uint32_t>("uint32_t");
    QSignalSpy spy(&dataSetTimer, SIGNAL(timeoutSignal(uint32_t)));

    QVERIFY(spy.wait(10000));
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(WRONG_TYPE_EXCEPTION);
  }
  catch(...)
  {
    QFAIL(WRONG_TYPE_EXCEPTION);
  }
}

void TsDataSetTimer::TestSchedulerSignalEmit()
{
  try
  {
    terrama2::core::DataSet dataSet("dummy", terrama2::core::DataSet::PCD_TYPE, 1);
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();

    te::dt::TimeDuration schedule(now.time_of_day().hours(), now.time_of_day().minutes()+1, 0);
    dataSet.setSchedule(schedule);
    terrama2::collector::DataSetTimer dataSetTimer(dataSet);

    qRegisterMetaType<uint32_t>("uint32_t");
    QSignalSpy spy(&dataSetTimer, SIGNAL(timeoutSignal(uint32_t)));

    QVERIFY(spy.wait(61000));
  }
  catch(boost::exception& e)
  {
    qDebug() << boost::get_error_info< terrama2::ErrorDescription >(e)->toStdString().c_str();
    QFAIL(WRONG_TYPE_EXCEPTION);
  }
  catch(...)
  {
    QFAIL(WRONG_TYPE_EXCEPTION);
  }
}
