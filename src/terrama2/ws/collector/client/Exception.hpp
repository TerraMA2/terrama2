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
  \file terrama2/Exception.hpp

  \brief Base exception for Collector WebService.

  \author Vinicius Campanha
 */

#ifndef __TERRAMA2_WS_COLLECTOR_CLIENT_EXCEPTION_HPP__
#define __TERRAMA2_WS_COLLECTOR_CLIENT_EXCEPTION_HPP__

// TerraMA2
#include "../Exception.hpp"

namespace terrama2
{
  namespace ws
  {
    namespace collector
    {
      namespace client
      {
        //! Exception to be used when receives a fault from WebService when request to add a DataProvider
        struct PingError: virtual terrama2::Exception{ };

        //! Exception to be used when receives a fault from WebService when request to add a DataProvider
        struct reloadError: virtual terrama2::Exception{ };

        //! Exception to be used when receives a fault from WebService when request to add a DataProvider
        struct AddingDataProviderError: virtual terrama2::Exception{ };
        
        //! Exception to be used when receives a fault from WebService when request to add a DataSet
        struct AddingDataSetError: virtual terrama2::Exception{ };

        //! Exception to be used when receives a fault from WebService when request to update a DataProvider
        struct UpdateDataProviderError: virtual terrama2::Exception{ };

        //! Exception to be used when receives a fault from WebService when request to update a DataSet
        struct UpdateDataSetError: virtual terrama2::Exception{ };

        //! Exception to be used when receives a fault from WebService when request to remove a DataProvider
        struct RemoveDataProviderError: virtual terrama2::Exception{ };

        //! Exception to be used when receives a fault from WebService when request to remove a DataSet
        struct RemoveDataSetError: virtual terrama2::Exception{ };

        //! Exception to be used when receives a fault from WebService when request to find a DataProvider
        struct FindDataProviderError: virtual terrama2::Exception{ };

        //! Exception to be used when receives a fault from WebService when request to find a DataSet
        struct FindDataSetError: virtual terrama2::Exception{ };

        //! Exception to be used when receives a fault from WebService when request to find a DataSet
        struct ListDataProviderError: virtual terrama2::Exception{ };

        //! Exception to be used when receives a fault from WebService when request to find a DataSet
        struct ListDataSetError: virtual terrama2::Exception{ };

      }
    }
  }

}  // end namespace terrama2

#endif  // __TERRAMA2_WS_COLLECTOR_CLIENT_EXCEPTION_HPP__
