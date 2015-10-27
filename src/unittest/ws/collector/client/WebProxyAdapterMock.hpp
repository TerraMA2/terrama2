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
  \file terrama2/unittest/ws/collector/client/WebProxyAdapterMock.hpp

  \brief Mock for WebProxyAdapter.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_UNITTEST_WS_COLLECTOR_WEBPROXYADAPTEREMOCK__
#define __TERRAMA2_UNITTEST_WS_COLLECTOR_WEBPROXYADAPTEREMOCK__


// TerraMA2
#include <soapWebProxy.h>
#include <terrama2/ws/collector/client/WebProxyAdapter.hpp>

// GMock
#include <gmock/gmock.h>

class WebProxyAdapterMock : public terrama2::ws::collector::client::WebProxyAdapter
{
  public:

    MOCK_METHOD1(ping, int(std::string &answer));
    MOCK_METHOD0(destroy, void());
    MOCK_METHOD2(addDataProvider, int(struct DataProvider dataProvider, struct DataProvider& dataProviderResult));
    MOCK_METHOD2(addDataSet, int(struct DataSet dataset, struct DataSet& datasetResult));
    MOCK_METHOD2(updateDataProvider, int(struct DataProvider dataProvider, struct DataProvider& dataProviderResult));
    MOCK_METHOD2(updateDataSet, int(struct DataSet dataset, struct DataSet& datasetResult));
    MOCK_METHOD1(removeDataProvider, int(uint64_t id));
    MOCK_METHOD0(recvRemoveDataProviderEmptyResponse, int());
    MOCK_METHOD1(removeDataSet, int(uint64_t id));
    MOCK_METHOD0(recvRemoveDatasetEmptyResponse, int());
    MOCK_METHOD2(findDataProvider, int(uint64_t id, struct DataProvider& dataProviderResult));
    MOCK_METHOD2(findDataSet, int(uint64_t id, struct DataSet& datasetResult));
    MOCK_METHOD1(listDataProvider, int(std::vector<struct DataProvider>& providers));
    MOCK_METHOD1(listDataSet, int(std::vector<struct DataSet>& datasets));
    MOCK_METHOD0(faultString, std::string());
    MOCK_METHOD0(faultDetail, std::string());
};
#endif //__TERRAMA2_UNITTEST_WS_COLLECTOR_WEBPROXYADAPTEREMOCK__
