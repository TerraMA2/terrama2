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
  \file terrama2/ws/collector/client/Client.hpp

  \brief Client header of TerraMA2 Collector Web Service.

  \author Vinicius Campanha
 */

#ifndef __TERRAMA2_WS_COLLECTOR_CLIENT_HPP__
#define __TERRAMA2_WS_COLLECTOR_CLIENT_HPP__

// STL
#include <string>

// gSOAP
#include "soapWebProxy.h"

// TerraMA2
#include "../../../core/DataProvider.hpp"
#include "../../../core/DataSet.hpp"

namespace terrama2
{
  namespace ws
  {
    namespace collector
    {
      namespace client
      {
        // Forward declaration
        class WebProxyAdapter;

        class Client
        {
          public:

            Client(WebProxyAdapter* webProxy);

            ~Client();


            /*!
              \brief Test server connection, after method the given string will hava the Web Service answer

              \param answer After method the given string will hava the Web Service answer

              \exception pingError

            */
            void ping(std::string &answer);


            /*!
              \brief Send shutdown signal to Web Service

              \exception ShutdownError

            */
            void shutdown();


            /*!
              \brief Request the addition of a DataProvider to Web Service

              \param dataProvider A Data Provider with data to be added, at the end it will contains a valid Data Provider

              \exception AddingDataProviderError

            */
            void addDataProvider(terrama2::core::DataProvider & dataProvider);

            /*!
              \brief Request the addition of a DataSet to Web Service

              \param dataSet A Data Set with data to be added, at the end it will contains a valid Data Set

              \exception AddingDataSetError

            */
            void addDataSet(terrama2::core::DataSet & dataSet);

            /*!
              \brief Request the update of a DataProvider to Web Service

              \param dataProvider A Data Provider with data to be updated, at the end it will contains a valid Data Provider

              \exception UpdateDataProviderError

            */
            void updateDataProvider(terrama2::core::DataProvider & dataProvider);

            /*!
              \brief Request the update of a Data Set to Web Service

              \param A Data Set with data to be updated, at the end it will contains a valid Data Provider

              \exception UpdateDataSetError

            */
            void updateDataSet(terrama2::core::DataSet & dataSet);

            /*!
              \brief Request the remove of a DataProvider to Web Service

              \param id The ID of a Data Provider to be removed

              \exception RemoveDataProviderError

            */
            void removeDataProvider(uint64_t id);

            /*!
              \brief Request the remove of a Data Set to Web Service

              \param id The ID of a Data Set to be removed

              \exception RemoveDataSetError
            */
            void removeDataSet(uint64_t id);

            /*!
              \brief Request to Web Service the Data Provider with the given ID

              \param id The ID of the wanted Data Provider

              \exception FindDataProviderError

              \return A terrama2::core::DataProvider
            */
            core::DataProvider findDataProvider(uint64_t id);

            /*!
              \brief Request to Web Service the Data Set with the given ID

              \param id The ID of the wanted Data Set

              \exception FindDataProviderError

              \return A terrama2::core::DataSet
            */
            core::DataSet findDataSet(uint64_t id);

            /*!
              \brief This method will fill the given vector with all terrama2::core::DataProvider registered

              \param dataProviderPtrList A vector of terrama2::core::DataProvider

              \exception ListDataProviderError

            */
            void listDataProvider(std::vector< terrama2::core::DataProvider > &dataProviderPtrList);

            /*!
              \brief This method will fill the given vector with all terrama2::core::DataSet registered

              \param dataSetPtrList A vector of terrama2::core::DataSet

              \exception ListDataSetError

            */
            void listDataSet(std::vector< terrama2::core::DataSet > &dataSetPtrList);


          private:

            WebProxyAdapter* webProxy_;

        };
      }
    }
  }
}

#endif // __TERRAMA2_WS_COLLECTOR_CLIENT_HPP__
