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
  te::da::MockDataSet* mockDataSet(new te::da::MockDataSet());

  EXPECT_CALL(*mockDataSet, moveNext()).WillRepeatedly(::testing::Return(true));
  EXPECT_CALL(*mockDataSet, getAsString(::testing::An<const std::string&>(),::testing::_)).WillRepeatedly(::testing::Return(""));
  EXPECT_CALL(*mockDataSet, getAsString(::testing::An<std::size_t>(),::testing::_)).WillRepeatedly(::testing::Return(""));
  EXPECT_CALL(*mockDataSet, isNull(std::string())).WillRepeatedly(::testing::Return(false));
  EXPECT_CALL(*mockDataSet, getNumProperties()).WillRepeatedly(::testing::Return(0));
  EXPECT_CALL(*mockDataSet, moveFirst()).WillRepeatedly(::testing::Return(true));

  return mockDataSet;
}

/*!
 * \brief This method creates a MockDataSourceTransactor
 * \return Returns a mocked DataSourceTransactor that has the required behavior for unittests
 */
te::da::MockDataSourceTransactor* createMockDataSourceTransactor()
{
  te::da::MockDataSourceTransactor* mockDataSourceTransactor(new te::da::MockDataSourceTransactor());

  EXPECT_CALL(*mockDataSourceTransactor, createDataSet(::testing::_, ::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*mockDataSourceTransactor, PrimaryKeyPtrReturn()).WillRepeatedly(::testing::Return(new te::da::PrimaryKey()));
  EXPECT_CALL(*mockDataSourceTransactor, execute(::testing::An<const std::string&>())).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*mockDataSourceTransactor, commit()).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*mockDataSourceTransactor, getLastGeneratedId()).WillRepeatedly(::testing::Return(1));
  EXPECT_CALL(*mockDataSourceTransactor, PropertyPtrReturn()).WillRepeatedly(::testing::Return(nullptr));
  EXPECT_CALL(*mockDataSourceTransactor, addPrimaryKey(::testing::An<const std::string&>(), ::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*mockDataSourceTransactor, addForeignKey(::testing::An<const std::string&>(), ::testing::_)).WillRepeatedly(::testing::Return());
  EXPECT_CALL(*mockDataSourceTransactor, escape(::testing::An<const std::string&>())).WillRepeatedly(::testing::Return(""));
  EXPECT_CALL(*mockDataSourceTransactor, isInTransaction()).WillRepeatedly(::testing::Return(false));
  EXPECT_CALL(*mockDataSourceTransactor, begin()).WillRepeatedly(::testing::Return());


  /* Every time the mockDataSourceTransactor object calls a method that returns a DataSet
   * the actualy method called will be the createMockDataSet() that returns a
   * new mocked DataSet.
   * A new mocked object is needed in every call because TerraLib takes ownership from the pointer.
   */
  EXPECT_CALL(*mockDataSourceTransactor, DataSetPtrReturn()).WillRepeatedly(::testing::Invoke(&createMockDataSet));

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
    TestLogger(te::da::MockDataSource* mockDataSource)
      : ProcessLogger()
    {
      setDataSource(mockDataSource);
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

    virtual std::shared_ptr<ProcessLogger> clone() const override {return nullptr; };
};


#endif // __TERRAMA2_UNITTEST_CORE_TESTLOGGER_HPP__
