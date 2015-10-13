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

  \brief Metadata about a dataset item.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_CORE_DATASETITEM_HPP__
#define __TERRAMA2_CORE_DATASETITEM_HPP__

// STL
#include <map>
#include <memory>
#include <string>

// Boost
#include <boost/noncopyable.hpp>

namespace terrama2
{
  namespace core
  {
// Forward declaration
    class DataSet;
    class Filter;

    /*!
      \class DataSetItem

      \brief Metadata about a dataset item.

      A dataset item can be a INPE Format PCD, TOA5 PCD, an occurrence of fire or an occurrence of diseases.
     */
    class DataSetItem : public boost::noncopyable
    {
      public:

        //! Dataset item type.
        enum Kind
        {
          UNKNOWN_TYPE,
          PCD_INPE_TYPE,
          PCD_TOA5_TYPE,
          FIRE_POINTS_TYPE,
          DISEASE_OCCURRENCE_TYPE
        };

        //! Dataset item status.
        enum Status
        {
          ACTIVE,
          INACTIVE
        };

      public:

        /*!
          \brief Constructor.

          \param id The dataset item identifier or zero if it doesn't have a valid one.
          \param k  The type of dataset item: PCD-INPE, PCD-TOA5, FIRE-POINTS, ...
          \param d  The dataset to which this item belongs to.
        */
        DataSetItem(Kind k = UNKNOWN_TYPE, const DataSet* d = nullptr, const uint64_t id = 0);

        /*! \brief Destructor. */
        ~DataSetItem();

        /*! \brief Returns the identifier of the dataset item. */
        uint64_t id() const;

        /*! \brief Sets the identifier of the dataset item. */
        void setId(uint64_t id);

        /*! \brief Returns the kind of the dataset item. */
        Kind kind() const;

        /*! \brief Sets the kind of the dataset item. */
        void setKind(const Kind k);

        /*! \brief Returns the status of the dataset item. */
        Status status() const;

        /*! \brief Sets the the status of the dataset item. */
        void setStatus(const Status s);

        /*! \brief Returns the mask of the dataset item. */
        const std::string& mask() const;

        /*! \brief Sets the mask of the dataset item. */
        void setMask(const std::string& m);

        /*! \brief Returns the timezone of the dataset item. */
        const std::string& timezone() const;

        /*! \brief Sets the timezone of the dataset item. */
        void setTimezone(const std::string& tz);

        /*! \brief Returns the dataset to which this item belongs to. */
        const DataSet* dataset() const;

        /*! \brief Returns the filter to be used when collecting this data item. */
        const Filter* filter() const;

        /*! \brief Sets the filter to be used when collecting data. */
        void setFilter(std::unique_ptr<Filter> f);

        /*! \brief Returns the storage strategy metadata. */
        const std::map<std::string, std::string>& storageMetadata() const;

        /*! \brief Returns the storage strategy metadata. */
        std::map<std::string, std::string>& storageMetadata();

        /*! \brief Sets the storage strategy metadata. */
        void setStorageMetadata(const std::map<std::string, std::string>& sm);

      private:

        Kind kind_;
        const DataSet* dataset_;
        uint64_t id_;
        Status status_;
        std::string mask_;
        std::string timezone_;
        std::unique_ptr<Filter> filter_;
        std::map<std::string, std::string> storageMetadata_;
    };

    typedef std::shared_ptr<DataSetItem> DataSetItemPtr;

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATASETITEM_HPP__

