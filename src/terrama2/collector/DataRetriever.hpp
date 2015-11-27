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
  \file terrama2/collector/DataRetriever.hpp

  \brief Interface for getting remote data to a local temporary file.

  \author Jano Simas
*/


#ifndef __TERRAMA2_COLLECTOR_DATARETRIEVER_HPP__
#define __TERRAMA2_COLLECTOR_DATARETRIEVER_HPP__

#include <memory>
#include <cassert>

#include "../core/DataProvider.hpp"

namespace terrama2
{
  namespace collector
  {
    class DataFilter;
    typedef std::shared_ptr<DataFilter> DataFilterPtr;

    /*!
      \brief The DataRetriever class should be used as a base class for retrieving remote data from servers. ex: FTP servers.

      The default behavior is isOpen = true and retrieveData = dataprovider.uri

     */
    class DataRetriever
    {
    public:
      /*!
        \brief Constructor, store DataProvider information.
        \param dataprovider DataProvider information

        \exception InvalidDataProviderError Raised when dataprovider doesn't have an id or a name.
       */
      explicit DataRetriever(const core::DataProvider& dataprovider);

      //! Does nothing. In derived classes opens the connectin to the server.
      virtual void open();
      //! Always returns true. In derived classes checks the connectin to the server.
      virtual bool isOpen();
      //! Does nothing. In derived classes closes the connection to the server.
      virtual void close();
      /*!
       * \brief Returns DataProvider uri. In derived classes retrieves remote data to a local temporary archive.
       * \param Filter to the data files.
       * \return Returns a standard Uniform Resource Identifier to the data.
       */
      virtual std::string retrieveData(DataFilterPtr filter);

    protected:
        terrama2::core::DataProvider dataprovider_;

    };

    typedef std::shared_ptr<DataRetriever> DataRetrieverPtr;
  }
}

#endif //__TERRAMA2_COLLECTOR_DATARETRIEVER_HPP__
