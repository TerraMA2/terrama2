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

/*!
 * \brief This method creates a MockDataSet
 * \return Returns a mocked DataSet that has the required behavior for unittests
 */
te::da::MockDataSet* createMockDataSet()
{
  te::da::MockDataSet* mockDataSet(new ::testing::NiceMock<te::da::MockDataSet>());

  ON_CALL(*mockDataSet, moveNext()).WillByDefault(::testing::Return(true));
  ON_CALL(*mockDataSet, getAsString(std::string(),::testing::_)).WillByDefault(::testing::Return(""));
  ON_CALL(*mockDataSet, isNull(std::string())).WillByDefault(::testing::Return(false));

  return mockDataSet;
}

/*!
 * \brief This method creates a MockDataSourceTransactor
 * \return Returns a mocked DataSourceTransactor that has the required behavior for unittests
 */
te::da::MockDataSourceTransactor* createMockDataSourceTransactor()
{
  te::da::MockDataSourceTransactor* mockDataSourceTransactor(new ::testing::NiceMock<te::da::MockDataSourceTransactor>());

  ON_CALL(*mockDataSourceTransactor, createDataSet(::testing::_, ::testing::_)).WillByDefault(::testing::Return());
  ON_CALL(*mockDataSourceTransactor, PrimaryKeyPtrReturn()).WillByDefault(::testing::Return(new te::da::PrimaryKey()));
  ON_CALL(*mockDataSourceTransactor, execute(std::string())).WillByDefault(::testing::Return());
  ON_CALL(*mockDataSourceTransactor, commit()).WillByDefault(::testing::Return());
  ON_CALL(*mockDataSourceTransactor, getLastGeneratedId()).WillByDefault(::testing::Return(1));

  /* Every time the mockDataSourceTransactor object calls a method that returns a DataSet
   * the actualy method called will be the createMockDataSet() that returns a
   * new mocked DataSet.
   * A new mocked object is needed in every call because TerraLib takes ownership from the pointer.
   */
  ON_CALL(*mockDataSourceTransactor, DataSetPtrReturn()).WillByDefault(::testing::Invoke(&createMockDataSet));

  return mockDataSourceTransactor;
}

class TestLogger : public terrama2::core::ProcessLogger
{
public:

  /*!
   * \brief Class constructor
   *
   * It will initialize a mock for DataSource class and set it in ProcessLogger class.
   */
  TestLogger()
    : ProcessLogger()
  {
    std::unique_ptr< te::da::MockDataSource > mockDataSource(new ::testing::NiceMock<te::da::MockDataSource>());

    ON_CALL(*mockDataSource.get(), isOpened()).WillByDefault(::testing::Return(true));
    ON_CALL(*mockDataSource.get(), dataSetExists(::testing::_)).WillByDefault(::testing::Return(false));

    /* Every time the mockDataSource object calls a method that returns a DataSourceTransactor
     * the actualy method called will be the createMockDataSourceTransactor() that returns a
     * new mocked DataSourceTransactor.
     * A new mocked object is needed in every call because TerraLib takes ownership from the pointer.
     */
    ON_CALL(*mockDataSource.get(), DataSourceTransactoPtrReturn()).WillByDefault(::testing::Invoke(&createMockDataSourceTransactor));


    setDataSource(mockDataSource.release());
    std::string tableName = "unittest"+std::to_string(1);
    setTableName(tableName);
  }

  /*!
   * \brief Class destructor
   */
  virtual ~TestLogger() = default;

  /*!
   * \brief The method addValue is protected in ProcessLog, so was needed to implement a method
   * that calls it for unittests.
   */
  void logValue(const std::string tag, const std::string value, const RegisterId registerId) const
  {
    addValue(tag, value, registerId);
  }
};


#endif // __TERRAMA2_UNITTEST_CORE_TESTLOGGER_HPP__
