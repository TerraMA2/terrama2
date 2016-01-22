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
  \file unittest/core/TsSerializer.cpp
  
  \brief Test for Serializer class
  
  \author Paulo R. M. Oliveira
*/

//TerraMA2
#include <terrama2/core/DataProvider.hpp>
#include <terrama2/core/DataSet.hpp>
#include <terrama2/core/DataSetItem.hpp>
#include <terrama2/core/Intersection.hpp>
#include <terrama2/core/Filter.hpp>

//QT
#include <QtTest/QTest>


class TsSerializer : public QObject
{
    Q_OBJECT
    
public:
  terrama2::core::Intersection createIntersection();
  terrama2::core::Filter createFilter();
  terrama2::core::DataSetItem createDataSetItem();
  terrama2::core::DataSet createDataSet();
  terrama2::core::DataProvider createDataProvider();

private slots:
    void testIntersection();
    void testFilter();
    void testDataSetItem();
    void testDataSet();
    void testDataProvider();
    
};
