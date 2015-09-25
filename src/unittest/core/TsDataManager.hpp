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

    //Data provider methods
    void testAddDataProvider();
    void testAddDataProviderWithId();
    void testAddDataProviderWithDataSet();

    void testRemoveDataProvider();
    void testRemoveDataProviderInvalidId();

    void testFindDataProvider();

    void testUpdateDataProvider();
    void testUpdateDataProviderInvalidId();

    //Dataset methods
    void testAddDataSet();
    void testAddDataSetWihId();

    void testRemoveDataSet();
    void testRemoveDataSetInvalidId();
    void testRemoveDataSetInUse();

    void testFindDataSet();

    void testUpdateDataSet();
    void testUpdateDataSetInvalidId();

    terrama2::core::DataProviderPtr createDataProvider();
    terrama2::core::DataSetPtr createDataSet();

};


