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
  \file terrama2/collector/TestDataSetTimer.cpp

  \brief Tests for the DataSetTimer class.

  \author Jano Simas
*/

#include "TestDataSetTimer.hpp"

#include <terrama2/core/DataSet.hpp>
#include <terrama2/collector/DataSetTimer.hpp>

//Qt
#include <QtTest>

void TestDataSetTimer::TestNullDataSet()
{
  terrama2::core::DataSetPtr nullDataSet;
  terrama2::collector::DataSetTimer nullDataSetTimer(nullDataSet);

  QVERIFY(!nullDataSetTimer.isValid());
}

void TestDataSetTimer::TestTimerSignalEmit()
{
  QEventLoop loop;
  QTimer timeout;
  connect(&timeout, SIGNAL(timeout()), &loop, SLOT(quit()));

  terrama2::core::DataSetPtr dataSet;//TODO: create a valid dataset
  terrama2::collector::DataSetTimer dataSetTimer(dataSet);

  QSignalSpy spy(&dataSetTimer, SIGNAL(timerSignal(uint64_t)));
  connect(&dataSetTimer, SIGNAL(timerSignal(uint64_t)), &loop, SLOT(quit()));

  timeout.start(20000);

  loop.exec(); //blocks untill either timerSignal or timeout was fired

  QCOMPARE(spy.count(), 1);
}


QTEST_MAIN(TestDataSetTimer)
#include "TestDataSetTimer.moc"
