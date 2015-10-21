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

      class Client
      {
        public:

        /*!
          \brief Constructor

          \param

          \return
        */
          Client(const std::string url);

          /*!
            \brief Destructor

            \param

            \return
          */
          ~Client();


          /*!
            \brief

            \param

            \exception pingError

            \return
          */
          void ping(std::string &answer);

          /*!
            \brief


          /*!
            \brief

            \param

            \exception AddingDataProviderError

            \return
          */
          void addDataProvider(terrama2::core::DataProvider & dataProvider);

          /*!
            \brief

            \param

            \exception AddingDataSetError

            \return
          */
          void addDataSet(terrama2::core::DataSet & dataSet);

          /*!
            \brief

            \param

            \exception UpdateDataProviderError

            \return
          */
          void updateDataProvider(terrama2::core::DataProvider & dataProvider);

          /*!
            \brief

            \param

            \exception UpdateDataSetError

            \return
          */
          void updateDataSet(terrama2::core::DataSet & dataSet);

          /*!
            \brief

            \param

            \exception RemoveDataProviderError

            \return
          */
          void removeDataProvider(uint64_t id);

          /*!
            \brief

            \param

            \exception RemoveDataSetError

            \return
          */
          void removeDataSet(uint64_t id);

          /*!
            \brief

            \param

            \exception FindDataProviderError

            \return
          */
          core::DataProvider findDataProvider(uint64_t id);

          /*!
            \brief

            \param

            \exception FindDataSetError

            \return
          */
          core::DataSet findDataSet(uint64_t id);

          /*!
            \brief

            \param

            \return
          */
          void listDataProvider(std::vector< terrama2::core::DataProvider > &dataProviderPtrList);

          /*!
            \brief

            \param

            \return
          */
          void listDataSet(std::vector< terrama2::core::DataSet > &dataSetPtrList);


        private:

          WebProxy* wsClient_;
          std::string server_;

    };

    }
  }

}

#endif // __TERRAMA2_WS_COLLECTOR_CLIENT_HPP__
