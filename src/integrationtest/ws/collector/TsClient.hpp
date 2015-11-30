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
  \file integrationtest/ws/collector/TsClient.hpp

  \brief Tests for the WebService Client class.

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_UNITTEST_WS_COLLECTOR_CLIENT__
#define __TERRAMA2_UNITTEST_WS_COLLECTOR_CLIENT__

// Qt
#include <QtTest>

// TerraMA2
#include <terrama2/ws/collector/client/Client.hpp>
#include <terrama2/ws/collector/client/WebProxyAdapter.hpp>


class TsClient: public QObject
{
  Q_OBJECT

private:
  terrama2::ws::collector::client::WebProxyAdapter* adapter_;
  terrama2::ws::collector::client::Client* wsClient_;

private:

    /*!
     * \brief Clear all the data in TerraMA2 database
     */
    void clearDatabase();

    terrama2::core::DataProvider buildDataProvider();
    terrama2::core::DataSet buildDataSet(uint64_t dataProvider_id);

private slots:

    void initTestCase(); // Run before all tests
    void cleanupTestCase(); // Run after all tests

    void init(); //run before each test
    void cleanup(); //run before each test

    //******Test functions********;


    /*!
     * \brief Test to request the WebService status
     */
    void TestStatus();


    /*!
     * \brief Test to request the WebService status
     */
    void TestWrongConection();


    /*!
     * \brief Test to add a normal DataProvider
     */
    void TestAddDataProvider();


    /*!
     * \brief Test to add a null DataProvider
     */
    void TestAddNullDataProvider();


    /*!
     * \brief
     */
    void testRemoveDataProvider();

    /*!
     * \brief
     */
    void testRemoveDataProviderInvalidId();


    /*!
     * \brief
     */
    void testUpdateDataProvider();


    /*!
     * \brief
     */
    void testUpdateDataProviderWrongID();


    /*!
     * \brief
     */
    void testFindDataProvider();


    /*!
     * \brief
     */
    void testFindDataProviderInvalidID();


    /*!
     * \brief
     */
    void testListDataProvider();


    /*!
     * \brief Test to add a normal DataSet
     */
    void TestAddDataSet();

    /*!
     * \brief Test to add a null DataProvider
     */
    void TestAddNullDataSet();


    /*!
     * \brief Test to add a DataProvider with an id
     */
    void TestAddDataSetWithID();


    /*!
     * \brief Test to add a DataProvider with an id
     */
    void TestAddDataSetWithWrongDataProviderID();


    /*!
     * \brief
     */
    void testRemoveDataSet();

    /*!
     * \brief
     */
    void testRemoveDataSetInvalidId();


    /*!
     * \brief
     */
    void testUpdateDataSet();

    /*!
     * \brief
     */
    void testUpdateDataSetInvalidId();


    /*!
     * \brief
     */
    void testFindDataSet();


    /*!
     * \brief
     */
    void testFindDataSetInvalidID();


    /*!
     * \brief
     */
    void testListDataSet();


    /*!
     * \brief
     */
    void testShutdownWebService();

    //******End of Test functions****



};

#endif // __TERRAMA2_UNITTEST_WS_COLLECTOR_CLIENT__

