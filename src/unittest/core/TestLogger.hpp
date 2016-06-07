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
  \file unittest/core/TestLogger.hpp

  \brief Logger class for tests

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_UNITTEST_CORE_TESTLOGGER_HPP__
#define __TERRAMA2_UNITTEST_CORE_TESTLOGGER_HPP__


//TerraMA2
#include <terrama2/core/utility/ProcessLogger.hpp>

#include "MockDataSource.hpp"
#include "MockDataSourceTransactor.hpp"
#include "MockDataSet.hpp"

te::da::MockDataSet* createMockDataSet()
{
  te::da::MockDataSet* mockDataSet(new te::da::MockDataSet());

  ON_CALL(*mockDataSet, moveNext()).WillByDefault(::testing::Return(true));
  ON_CALL(*mockDataSet, getAsString(std::string(),::testing::_)).WillByDefault(::testing::Return(""));
  ON_CALL(*mockDataSet, isNull(std::string())).WillByDefault(::testing::Return(false));

  return mockDataSet;
}

te::da::MockDataSourceTransactor* createMockDataSourceTransactor()
{
  te::da::MockDataSourceTransactor* mockDataSourceTransactor(new te::da::MockDataSourceTransactor());

  ON_CALL(*mockDataSourceTransactor, createDataSet(::testing::_, ::testing::_)).WillByDefault(::testing::Return());
  ON_CALL(*mockDataSourceTransactor, PrimaryKeyPtrReturn()).WillByDefault(::testing::Return(new te::da::PrimaryKey()));
  ON_CALL(*mockDataSourceTransactor, execute(std::string())).WillByDefault(::testing::Return());
  ON_CALL(*mockDataSourceTransactor, commit()).WillByDefault(::testing::Return());
  ON_CALL(*mockDataSourceTransactor, getLastGeneratedId()).WillByDefault(::testing::Return(1));
  ON_CALL(*mockDataSourceTransactor, DataSetPtrReturn()).WillByDefault(::testing::Invoke(&createMockDataSet));


  return mockDataSourceTransactor;
}

class TestLogger : public terrama2::core::ProcessLogger
{
public:
  TestLogger()
    : ProcessLogger()
  {
    // Mock
    std::unique_ptr< te::da::MockDataSource > mockDataSource(new te::da::MockDataSource());

    ON_CALL(*mockDataSource.get(), dataSetExists(::testing::_)).WillByDefault(::testing::Return(false));
    ON_CALL(*mockDataSource.get(), DataSourceTransactoPtrReturn()).WillByDefault(::testing::Invoke(&createMockDataSourceTransactor));

    setDataSource(mockDataSource.release());

    setTableName("unittest"+std::to_string(1));

  }

  virtual ~TestLogger() = default;

  void logValue(const std::string tag, const std::string value, const RegisterId registerId) const
  {
    addValue(tag, value, registerId);
  }
};


#endif // __TERRAMA2_UNITTEST_CORE_TESTLOGGER_HPP__
