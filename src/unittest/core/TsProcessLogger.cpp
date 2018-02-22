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
  \file unittest/core/TsProcessLogger.cpp

  \brief Tests for Core Process Logger class

  \author Vinicius Campanha
*/

//TerraMA2
#include <terrama2/core/Typedef.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>
#include <terrama2/Exception.hpp>
#include "TsProcessLogger.hpp"
#include "TestLogger.hpp"



void TsProcessLogger::testProcessLogger()
{
  try
  {
    std::unique_ptr<te::da::MockDataSource> mockDataSource(new ::testing::NiceMock<te::da::MockDataSource>());

    EXPECT_CALL(*mockDataSource, open()).WillRepeatedly(::testing::Return());
    EXPECT_CALL(*mockDataSource, isOpened()).WillRepeatedly(::testing::Return(true));
    EXPECT_CALL(*mockDataSource, dataSetExists(::testing::_)).WillRepeatedly(::testing::Return(false));

    /* Every time the mockDataSource object calls a method that returns a DataSourceTransactor
     * the actualy method called will be the createMockDataSourceTransactor() that returns a
     * new mocked DataSourceTransactor.
     * A new mocked object is needed in every call because TerraLib takes ownership from the pointer.
     */
    EXPECT_CALL(*mockDataSource, DataSourceTransactoPtrReturn()).WillRepeatedly(::testing::Invoke(&createMockDataSourceTransactor));

    TestLogger log(mockDataSource.release());

    RegisterId registerID = log.start(1);

    log.logValue("tag1", "value1", registerID);
    log.logValue("tag2", "value2", registerID);
    log.logValue("tag1", "value3", registerID);
    log.logValue("tag2", "value4", registerID);
    log.log(TestLogger::MessageType::ERROR_MESSAGE, "Unit Test Error", registerID);
    log.log(TestLogger::MessageType::ERROR_MESSAGE, "Unit Test second Error", registerID);
    log.log(TestLogger::MessageType::INFO_MESSAGE, "Unit Test Info", registerID);
    log.log(TestLogger::MessageType::INFO_MESSAGE, "Unit Test seconde Info", registerID);

    std::shared_ptr< te::dt::TimeInstantTZ > dataTime = terrama2::core::TimeUtils::nowUTC();

    log.result(terrama2::core::ProcessLogger::Status::ERROR, dataTime, registerID);
  }
  catch(const terrama2::Exception& e)
  {
    QFAIL(e.what());
  }
}
