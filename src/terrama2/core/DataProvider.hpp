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

  \brief Models the information of a data provider (or data server).

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

      \brief Models the information of a data provider (or data server).

      A data provider can be a remote server that provides data through
      FTP protocol or an OGC Web Service, such as WFS, WCS or SOS.

      It can also be an URI for a folder into the file system.

      A data provider contains the list of datasets that belongs to this provider 
      that should be collected for further analysis.
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
          WCS_TYPE,
          SOS_TYPE
        };

        //! Data provider status.
        enum Status
        {
          ACTIVE,
          INACTIVE
        };

        /*! \brief Constructor. */
        DataProvider(const uint64_t id = 0, Kind k = UNKNOWN_TYPE);

        /*! \brief Destructor. */
        ~DataProvider();

        /*! \brief Returns the identifier of the data provider. */
        uint64_t id() const;

        /*! \brief Sets the identifier of the data provider. */
        void setId(uint64_t id);

        /*! \brief Returns the name of the data provider. */
        const std::string& name() const;

        /*! \brief Sets the name of the data provider. */
        void setName(const std::string& name);

        /*! \brief Returns the description of the data provider. */
        const std::string& description() const;

        /*! \brief Sets the the description of the data provider. */
        void setDescription(const std::string& description);

        /*! \brief Returns the the kind of the data provider. */
        Kind kind() const;

        /*! \brief Sets the the kind of the data provider.  */
        void setKind(Kind k);

        /*! \brief Returns the URI of the data provider. */
        const std::string& uri() const;

        /*! \brief Sets the URI of the data provider. */
        void setUri(const std::string& uri);

        /*! \brief Returns the the status of the data provider. */
        Status status() const;

        /*! \brief Sets the the status of the data provider. */
        void setStatus(Status s);

        /*! \brief Returns a reference to the dataset list to be collected from this data provider. */
        const std::vector<DataSet>& datasets() const;

        /*!
          \brief Adds a new dataset to the data provider.

          \param d The the dataset.
        */
        void add(const DataSet& d);

        /*!
          \brief Removes the given dataset from the provider list.

          \param id The identifier of the dataset to be removed.
         */
        void removeDataSet(const uint64_t id);

      private:

        uint64_t id_;
        std::string name_;
        std::string description_;
        Kind kind_;
        std::string uri_;
        Status status_;
        std::vector<DataSet> datasets_; //!< The list of datasets available in the data provider.
    };

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATAPROVIDER_HPP__

