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


class TestLogger : public terrama2::core::ProcessLogger
{
public:
  TestLogger()
    : ProcessLogger()
  {
    std::shared_ptr< te::da::MockDataSource > mockDataSource;
    setDataSource(mockDataSource);

    std::auto_ptr<te::da::DataSourceTransactor> mockDataSourceTransactor(new te::da::MockDataSourceTransactor());
//    std::auto_ptr< te::da::MockDataSourceTransactor > mockDataSourceTransactor;

//    te::da::MockDataSourceTransactor mTransactor;
//    std::auto_ptr< te::da::DataSourceTransactor > mock(dynamic_cast<te::da::DataSourceTransactor*>(&mTransactor));

    ON_CALL(*mockDataSource.get(), getTransactor()).WillByDefault(::testing::Return(mockDataSourceTransactor));

  }

  virtual ~TestLogger() = default;

  void addValue(const std::string tag, const std::string value, const RegisterId registerId) const
  {
    addValue(tag, value, registerId);
  }
};


#endif // __TERRAMA2_UNITTEST_CORE_TESTLOGGER_HPP__
