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
  \file terrama2/ws/collector/client/WebProxyAdapter.hpp

  \brief Adapter for gSOAP WebProxy class.

  \author Paulo R. M. Oliveira
*/

#ifndef TERRAMA2_WS_COLLECTOR_CLIENT_WEBPROXYADAPTER_HPP
#define TERRAMA2_WS_COLLECTOR_CLIENT_WEBPROXYADAPTER_HPP

// gSOAP
#include "soapWebProxy.h"

// STL
#include <vector>
#include <string>

namespace terrama2
{
  namespace ws
  {
    namespace collector
    {
      namespace client
      {
        class WebProxyAdapter
        {
          public:
            WebProxyAdapter(const std::string server);

            virtual ~WebProxyAdapter();

            virtual void destroy();

            virtual int shutdown();

            virtual int recvShutdownEmptyResponse();

            virtual	int ping(std::string &answer);

            virtual	int addDataProvider(struct DataProvider dataProvider, struct DataProvider& dataProviderResult);

            virtual	int addDataSet(struct DataSet dataset, struct DataSet& datasetResult);

            virtual	int updateDataProvider(struct DataProvider dataProvider, struct DataProvider& dataProviderResult);

            virtual	int updateDataSet(struct DataSet dataset, struct DataSet& datasetResult);

            virtual	int removeDataProvider(uint64_t id);

            virtual int recvRemoveDataProviderEmptyResponse();

            virtual	int removeDataSet(uint64_t id);

            virtual int recvRemoveDatasetEmptyResponse();

            virtual	int findDataProvider(uint64_t id, struct DataProvider& dataProviderResult);

            virtual	int findDataSet(uint64_t id, struct DataSet& datasetResult);

            virtual	int listDataProvider(std::vector<struct DataProvider>& providers);

            virtual	int listDataSet(std::vector<struct DataSet>& datasets);

            virtual	std::string faultString();

            virtual	std::string faultDetail();

          protected:
            WebProxyAdapter();

          private:
            WebProxy* webProxy_;
        };
      }
    }
  }
}



#endif //TERRAMA2_WS_COLLECTOR_CLIENT_WEBPROXYADAPTER_HPP
