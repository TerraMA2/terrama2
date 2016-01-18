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
  \file terrama2/core/DataSetItem.hpp

  \brief Metadata about a DataSetItem.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_CORE_DATASETITEM_HPP__
#define __TERRAMA2_CORE_DATASETITEM_HPP__

// TerraLib
#include "Filter.hpp"

// STL
#include <map>
#include <string>

namespace terrama2
{
  namespace core
  {

    /*!
      \class DataSetItem

      \brief Metadata about a DataSetItem.

      A DataSetItem can be a INPE Format PCD, TOA5 PCD, an occurrence of fire or an occurrence of diseases.
     */
    class DataSetItem
    {
      public:

        /*! \brief DataSet item type.
         Each constant must exist in table terrama2.dataset_item_type and the value must be the same from column id.
         */
        enum Kind
        {
          UNKNOWN_TYPE = 1, //!< Unkown type
          PCD_INPE_TYPE = 2, //!< PCD-INPE type.
          PCD_TOA5_TYPE = 3, //!< PCD format TOA5.
          FIRE_POINTS_TYPE = 4, //!< Fire points data.
          DISEASE_OCCURRENCE_TYPE = 5, //!< Disease occurence data.
          GRID_TYPE = 6 //!< For sattelite images or grid data.
        };

        //! DataSet item status.
        enum Status
        {
          ACTIVE, //!< When active data will be collected.
          INACTIVE //!< When inactive data wont't be collected.
        };

      public:

        /*!
          \brief Constructor.

          \param k         The type of DataSetItem: PCD-INPE, PCD-TOA5, FIRE-POINTS, ...
          \param id        The DataSetItem identifier or zero if it doesn't have a valid one.
          \param datasetId The DataSet to which this item belongs to.
        */
        DataSetItem(Kind k = UNKNOWN_TYPE, uint64_t id = 0, uint64_t datasetId = 0);

        /*! \brief Destructor. */
        ~DataSetItem();

        /*! \brief Returns the identifier of the DataSetItem. */
        uint64_t id() const;

        /*! \brief Sets the identifier of the DataSetItem. */
        void setId(uint64_t id);

        /*! \brief Returns the kind of the DataSetItem. */
        Kind kind() const;

        /*! \brief Sets the kind of the DataSetItem. */
        void setKind(const Kind k);

        /*! \brief Returns the status of the DataSetItem. */
        Status status() const;

        /*! \brief Sets the the status of the DataSetItem. */
        void setStatus(const Status s);

        /*! \brief Returns the mask of the DataSetItem. */
        const std::string& mask() const;

        /*! \brief Sets the mask of the DataSetItem. */
        void setMask(const std::string& m);

        /*! \brief Returns the timezone of the DataSetItem. */
        const std::string& timezone() const;

        /*! \brief Sets the timezone of the DataSetItem. */
        void setTimezone(const std::string& tz);

        /*! \brief Returns the path to a DataSetItem. */
        std::string path() const;

        /*! \brief Sets the path to a DataSetItem. */
        void setPath(const std::string& path);

        /*! \brief Returns the DataSet to which this item belongs to. */
        uint64_t dataset() const;

        /*! \brief Sets the DataSet id */
        void setDataSet(uint64_t id);

        /*! \brief Returns the filter to be used when collecting this DataSetItem. */
        const Filter& filter() const;

        /*! \brief Sets the filter to be used when collecting DataSetItem. */
        void setFilter(const Filter& f);

        /*! \brief Returns the storage metadata. */
        const std::map<std::string, std::string>& metadata() const;

        /*! \brief Returns the storage metadata. */
        std::map<std::string, std::string>& metadata();

        /*! \brief Sets the storage metadata. */
        void setMetadata(const std::map<std::string, std::string>& metadata);

        /*! \brief Returns the spatial reference identifier. */
        uint64_t srid();

        /*! \brief Sets the spatial reference identifier. */
        void setSrid(const uint64_t srid);

      private:

        Kind kind_; //!< DataSet item type.
        uint64_t id_; //!< Identifier of the DataSetItem.
        uint64_t dataset_; //!< Identifier of the DataSetItem.
        Status status_; //!< DataSet item status for collection.
        std::string mask_; //!< Mask of the DataSetItem.
        std::string timezone_; //!< Timezone of the DataSetItem.
        std::string path_; //!< Path to a DataSetItem.
        Filter filter_; //!< Filter to be used when collecting this DataSetItem.
        std::map<std::string, std::string> metadata_; //!< Storage metadata.
        uint64_t srid_; //!< Projection SRID.
    };

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATASETITEM_HPP__

