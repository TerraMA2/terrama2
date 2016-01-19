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

  \brief Models the information of a DataProvider (or data server).

  \author Gilberto Ribeiro de Queiroz
  \author Jano Simas
  \author Paulo R. M. Oliveira
  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_CORE_DATAPROVIDER_HPP__
#define __TERRAMA2_CORE_DATAPROVIDER_HPP__

// TerraMA2
#include "DataSet.hpp"

// STL
#include <string>
#include <vector>

namespace terrama2
{
  namespace core
  {
    /*!
      \class DataProvider

      \brief Models the information of a DataProvider (or data server).

      A DataProvider can be a remote server that provides data through
      FTP protocol or an OGC Web Service, such as WFS, WCS or SOS.

      It can also be an URI for a folder into the file system.

      A DataProvider contains the list of datasets that belongs to this provider
      that should be collected for further analysis.
     */
    class DataProvider
    {
      public:

        /*! \brief DataProvider type.
         Each constant must exist in table terrama2.data_provider_type and the value must be the same from column id.
         */
        enum Kind
        {
          UNKNOWN_TYPE = 1, //!< Unkown type
          FTP_TYPE = 2, //!< FTP protocol.
          HTTP_TYPE = 3, //!< HTTP protocol.
          FILE_TYPE = 4, //!< Local files.
          WFS_TYPE = 5, //!< OGC Web Feature Service.
          WCS_TYPE = 6, //!< OGC Web Coverage Service.
          SOS_TYPE = 7, //!< OGC Sensor Observation Service.
          POSTGIS_TYPE = 8 //!< PostGIS database server.
        };

        /*! \brief DataProvider origin.
         Each constant must exist in table terrama2.data_provider_origin and the value must be the same from column id.
         */
        enum Origin
        {
          COLLECTOR = 1, //!< This data provider is used in the collector.
          ANALYSIS = 2 //!< This data provider is used in the analysis.
        };

        //! DataProvider status.
        enum Status
        {
          ACTIVE, //!< The data provider is available.
          INACTIVE //!< The data provider is not available.
        };

        /*! \brief Constructor. */
        DataProvider(const std::string& name = "", Kind k = UNKNOWN_TYPE, const uint64_t id = 0);

        /*! \brief Destructor. */
        ~DataProvider();

        /*! \brief Returns the identifier of the DataProvider. */
        uint64_t id() const;

        /*! \brief Sets the identifier of the DataProvider. */
        void setId(uint64_t id);

        /*! \brief Returns the name of the DataProvider. */
        const std::string& name() const;

        /*! \brief Sets the name of the DataProvider. */
        void setName(const std::string& name);

        /*! \brief Returns the description of the DataProvider. */
        const std::string& description() const;

        /*! \brief Sets the the description of the DataProvider. */
        void setDescription(const std::string& description);

        /*! \brief Returns the the kind of the DataProvider. */
        Kind kind() const;

        /*! \brief Sets the the kind of the DataProvider.  */
        void setKind(Kind k);

        /*! \brief Returns the the origin of the DataProvider. */
        Origin origin() const;

        /*! \brief Sets the the origin of the DataProvider.  */
        void setOrigin(Origin origin);

        /*! \brief Returns the URI of the DataProvider. */
        const std::string& uri() const;

        /*! \brief Sets the URI of the DataProvider. */
        void setUri(const std::string& uri);

        /*! \brief Returns the the status of the DataProvider. */
        Status status() const;

        /*! \brief Sets the the status of the DataProvider. */
        void setStatus(Status s);

        /*! \brief Returns a reference to the DataSet list to be collected from this DataProvider. */
        std::vector<DataSet>& datasets();

        /*! \brief Returns a reference to the DataSet list to be collected from this DataProvider. */
        const std::vector<DataSet>& datasets() const;

        /*!
          \brief Adds a new DataSet to the DataProvider.

          \param d The the DataSet.
        */
        void add(DataSet& d);

        /*!
          \brief Removes the given DataSet from the provider list.

          \param id The identifier of the DataSet to be removed.
         */
        void removeDataSet(const uint64_t id);

      private:

        uint64_t id_; //!< The identifier of the DataProvider.
        std::string name_; //!< Name of the DataProvider, must be unique.
        Kind kind_; //!< DataProvider type.
        Origin origin_; //! DataProvider origin.
        std::string description_; //!< Brief description from the source of the DataProvider.
        std::string uri_; //!< URI to access the DataProvider data.
        Status status_; //!< DataProvider status.
        std::vector<DataSet> datasets_; //!< The list of datasets available in the DataProvider.
    };

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATAPROVIDER_HPP__

