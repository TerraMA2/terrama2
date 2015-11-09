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
  \file terrama2/unittest/ws/client/TsCore.hpp

  \brief Tests for the WebService Core class.

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_UNITTEST_WS_COLLECTOR_CORE__
#define __TERRAMA2_UNITTEST_WS_COLLECTOR_CORE__

// Qt
#include <QtTest>

// TerraMA2
#include <terrama2/core/DataManager.hpp>


class TsCore: public QObject
{
  Q_OBJECT

private:

   /*!
   * \brief Create a Data Provider Ptr
   */
    terrama2::core::DataProvider buildDataProvider();


    /*!
     * \brief Create a Data Set Ptr
     */
    terrama2::core::DataSet buildDataSet();


    /*!
     * \brief Clear all the data in TerraMA2 database
     */
    void clearDatabase();

private slots:

    void initTestCase(){} // Run before all tests
    void cleanupTestCase(){} // Run after all tests

    void init(); //run before each test
    void cleanup(); //run before each test

    //******Test functions********;


    /*!
     * \brief Test to convert a DataProviderPtr to a Data Provider Struct
     */
    void TestConvertDataProviderToDataProviderStruct();


    /*!
     * \brief Test to convert a Data Provider Struct to a DataProviderPtr
     */
    void TestConvertDataProviderStructToDataProvider();


    /*!
     * \brief Test to convert a DataSetPtr to a Data Set Struct
     */
    void TestConvertDataSetToDataSetStruct();


    /*!
     * \brief Test to convert a Data Set Struct toa DataSetPtr
     */
    void TestConvertDataSetStructToDataSet();



};

#endif // __TERRAMA2_UNITTEST_WS_COLLECTOR_CORE__

