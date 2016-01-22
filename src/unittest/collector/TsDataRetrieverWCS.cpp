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
  \file terrama2/unittest/collector/TsDataRetrieverWCS.cpp

  \brief Tests for the collector Data Retriever WCS.

 \author Vinicius Campanha
*/

// TerraMA2
#include "TsDataRetrieverWCS.hpp"
#include "terrama2/core/DataProvider.hpp"
#include "terrama2/collector/DataRetrieverWCS.hpp"


void TsDataRetrieverWCS::initTestCase()
{

}


void TsDataRetrieverWCS::cleanupTestCase()
{

}


void TsDataRetrieverWCS::init()
{

}


void TsDataRetrieverWCS::cleanup()
{

}


void TsDataRetrieverWCS::TestIsOpen()
{
  terrama2::core::DataProvider dataprovider("DataProviderTest");

  dataprovider.setUri("http://flanche.net:9090/rasdaman/ows?SERVICE=WCS");

  terrama2::collector::DataRetrieverWCS dataRetriever(dataprovider);

  if(!dataRetriever.isOpen())
    QFAIL("Should not be here");
}

void TsDataRetrieverWCS::TestWrongAdress()
{
  terrama2::core::DataProvider dataprovider("DataProviderTest");

  dataprovider.setUri("http://wrongadress/ows?SERVICE=WCS");

  terrama2::collector::DataRetrieverWCS dataRetriever(dataprovider);

  if(dataRetriever.isOpen())
    QFAIL("Should not be here");
}

void TsDataRetrieverWCS::TestNoService()
{
  terrama2::core::DataProvider dataprovider("DataProviderTest");

  dataprovider.setUri("http://flanche.net:9090/rasdaman/ows?");

  terrama2::collector::DataRetrieverWCS dataRetriever(dataprovider);

  if(dataRetriever.isOpen())
    QFAIL("Should not be here");
}
