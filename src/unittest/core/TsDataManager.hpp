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
  \file unittest/core/TsDataManager.cpp

  \brief Test for data manager class

  \author Paulo R. M. Oliveira
*/

//TerraMA2
#include <terrama2/core/DataProvider.hpp>
#include <terrama2/core/DataSet.hpp>

//QT
#include <QtTest/QTest>

//STL
#include <memory>

namespace terrama2
{
  namespace core
  {
    class DataProvider;
    typedef std::shared_ptr<DataProvider> DataProviderPtr;

    class DataSet;
    typedef std::shared_ptr<DataSet> DataSetPtr;
  }
}

class TsDataManager : public QObject
{
  Q_OBJECT


  private:
    void clearDatabase();

  private slots:
    void init();
    void cleanup();

    void testLoad();
    void testUnload();

    //Data provider tests

    // Tests for add
    void testAddDataProvider();
    void testAddNullDataProvider();
    void testAddDataProviderWithId();
    void testAddDataProviderWithDataSet();

    // Tests for update
    void testUpdateDataProvider();
    void testUpdateNonexistentDataProvider();
    void testUpdateNullDataProvider();
    void testUpdateDataProviderInvalidId();

    // Tests for remove
    void testRemoveDataProvider();
    void testRemoveDataProviderInvalidId();
    void testRemoveNonExistentDataProvider();
    void testRemoveDataProviderWithDataSet();

    // Tests for find
    void testFindDataProvider();
    void testFindNonExistentDataProvider();
    void testFindDataProviderByName();

    void testDataProviderValidName();

    //Dataset tests

    // Tests for add
    void testAddDataSet();
    void testAddDataSetWihId();
    void testAddNullDataSet();
    void testAddDataSetWithNullProvider();
    void testAddDataSetWithNonexistentProvider();

    // Tests for update
    void testUpdateDataSet();
    void testUpdateNullDataSet();
    void testUpdateDataSetInvalidId();
    void testUpdateDataSetWithNullProvider();
    void testUpdateDataSetWithNonexistentProvider();

    // Tests for remove
    void testRemoveDataSet();
    void testRemoveDataSetInvalidId();
    void testRemoveDataSetInUse();
    void testRemoveNonExistentDataSet();

    // Tests for find
    void testFindDataSet();
    void testFindNonExistentDataSet();
    void testFindDataSetByName();

    void testDatasetValidName();


    terrama2::core::DataProvider createDataProvider();
    terrama2::core::DataSet createDataSet();


};
