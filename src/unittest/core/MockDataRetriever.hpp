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
  \file terrama2/unittest/core/MockDataRetriever.hpp
  \brief Mock Class DataRetriever
  \author Evandro Delatin
*/

#ifndef __TERRAMA2_UNITTEST_CORE_MOCKDATARETRIEVER__
#define __TERRAMA2_UNITTEST_CORE_MOCKDATARETRIEVER__

// TerraMA2
#include <terrama2/core/data-access/DataRetriever.hpp>

// Libcurl
#include <curl/curl.h>

// GMock
#include <gmock/gmock.h>

class MockDataRetriever: public terrama2::core::DataRetriever
{
  public:

    MockDataRetriever(terrama2::core::DataProviderPtr dataProvider) :
      DataRetriever(dataProvider) {}

    MOCK_METHOD6(retrieveData,std::string(const std::string& query,
                                          const terrama2::core::Filter& filter,
                                          const std::string& timezone,
                                          std::shared_ptr<terrama2::core::FileRemover> remover,
                                          const std::string& tempFolder,
                                          const std::string& folderPath));

    MOCK_CONST_METHOD0(lastDateTime,te::dt::TimeInstantTZ());

    MOCK_CONST_METHOD0(isRetrivable,bool());

    static terrama2::core::DataRetrieverPtr makeMockDataRetriever(terrama2::core::DataProviderPtr /*dataProvider*/, std::shared_ptr<MockDataRetriever> mock)
    {
      return mock;
    }
};

#endif //__TERRAMA2_UNITTEST_CORE_MOCKDATARETRIEVER__
