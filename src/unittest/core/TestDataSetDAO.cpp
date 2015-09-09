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
  \file unittest/core/DataSetDAO.cpp

  \brief Test for DataSetDAO functionalities

  \author Paulo R. M. Oliveira
*/

#include "TestDataSetDAO.hpp"

#include <terrama2/core/DataSet.hpp>
#include <terrama2/core/DataProvider.hpp>
#include <terrama2/core/ApplicationController.hpp>

//QT
#include <QtTest>



void TestDataSetDAO::testAddDataSet()
{
  terrama2::core::DataProviderPtr dataProvider(new terrama2::core::DataProvider("Server 1"));

  terrama2::core::DataSetPtr dataset(new terrama2::core::DataSet(dataProvider));
}



void TestDataSetDAO::testUpdateDataSet()
{

}


void TestDataSetDAO::testRemoveDataSet()
{

}


void TestDataSetDAO::testListDataSet()
{

}

#include "TestDataSetDAO.moc"
