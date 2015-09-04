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
  \file terrama2/core/DataProvider.hpp

  \brief Metadata of a data server.

  \author Gilberto Ribeiro de Queiroz
  \author Jano Simas
  \author Paulo R. M. Oliveira
  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_CORE_DATAPROVIDER_HPP__
#define __TERRAMA2_CORE_DATAPROVIDER_HPP__

// STL
#include <memory>
#include <string>

namespace terrama2
{
  namespace core
  {
    /*!
      \class DataProvider

      \brief Contains metadata about a data provider (or data server).

      A data provider can be a remote server that provides data through
      FTP protocol or an OGC Web Service, such as WFS, WCS or SOS.
      
      It can also be an URI for a folder into the file system...
     */
    class DataProvider
    {
      public:

      //! Data provider type.
      enum Kind
      {
        UNKNOWN_TYPE,
        FTP_TYPE,
        HTTP_TYPE,
        FILE_TYPE,
        WFS_TYPE,
        WCS_TYPE
      };

      //! Data provider status.
      enum Status
      {
        ACTIVE,
        INACTIVE
      };

      private:

        uint64 id_;
        std::string name_;
        std::string description_;
        Kind kind_;
        std::string uri_;
        Status status_;
        std::vector<DataSetPtr> datasets_; //!< The list of datasets available in the data provider.
    };

    typedef shared_ptr<DataProvider> DataProviderPtr;

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATAPROVIDER_HPP__

