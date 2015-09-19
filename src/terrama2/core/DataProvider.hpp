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
#include <vector>

namespace terrama2
{
  namespace core
  {
    class DataSet;
    typedef std::shared_ptr<DataSet> DataSetPtr;

    /*!
      \class DataProvider

      \brief Contains metadata about a data provider (or data server).

      A data provider can be a remote server that provides data through
      FTP protocol or an OGC Web Service, such as WFS, WCS or SOS.

      It can also be an URI for a folder into the file system.

      It contains a list of datasets that belongs to this provider.
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

        /*!
          \brief Constructor
        */
        DataProvider(const std::string& name, Kind kind);

        /*!
          \brief Destructor.
        */
        ~DataProvider();

        /*!
          \brief It returns the identifier of the data provider.

          \return The identifier of the data provider.
        */
        uint64_t id() const;

        /*!
          \brief It returns the name of the data provider.

          \return The name of the data provider.
        */
        std::string name() const;

        /*!
          \brief It sets the name of the data provider.

          \param The name of the data provider.
        */
        void setName(const std::string& name);

        /*!
          \brief It returns the the description of the data provider.

          \return The the description of the data provider.
        */
        std::string description() const;

        /*!
          \brief It sets the the description of the data provider.

          \param The the description of the data provider.
        */
        void setDescription(const std::string& description);

        /*!
          \brief It returns the the kind of the data provider.

          \return The the kind of the data provider.
        */
        Kind kind() const;

        /*!
          \brief It sets the the kind of the data provider.

          \param The the kind of the data provider.
        */
        void setKind(Kind kind);

        /*!
          \brief It returns the URI of the data provider.

          \return The URI of the data provider.
        */
        std::string uri() const;

        /*!
          \brief It sets the URI of the data provider.

          \param The URI of the data provider.
        */
        void setUri(const std::string& uri);

        /*!
          \brief It returns the the status of the data provider.

          \return The the status of the data provider.
        */
        Status status() const;

        /*!
          \brief It sets the the status of the data provider.

          \param The the status of the data provider.
        */
        void setStatus(Status status);

        /*!
          \brief It returns the the dataset list.

          \return The the dataset list.
        */
        std::vector<DataSetPtr> dataSets() const;

        /*!
          \brief It sets the the dataset list.

          \param The the dataset list.
        */
        void setDataSets(const std::vector<DataSetPtr>& dataSets);

      protected:

        /*!
          \brief It sets the identifier of the data provider.

          \param The identifier of the data provider.
        */
        void setId(uint64_t id);

      private:

        uint64_t id_;
        std::string name_;
        std::string description_;
        Kind kind_;
        std::string uri_;
        Status status_;
        std::vector<DataSetPtr> dataSets_; //!< The list of datasets available in the data provider.

      friend class DataProviderDAO; //review
    };

    typedef std::shared_ptr<DataProvider> DataProviderPtr;

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATAPROVIDER_HPP__

