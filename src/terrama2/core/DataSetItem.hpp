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

  \brief Metadata about a given dataset item.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_CORE_DATASETITEM_HPP__
#define __TERRAMA2_CORE_DATASETITEM_HPP__


// STL
#include <memory>
#include <string>
#include <map>

// Boost
#include <boost/noncopyable.hpp>

namespace terrama2
{
  namespace core
  {
// Forward declaration
    class DataSet;
    typedef std::shared_ptr<DataSet> DataSetPtr;

    class Filter;
    typedef std::shared_ptr<Filter> FilterPtr;

    /*!
      \class DataSetItem

      \brief Contains metadata about a dataset item.

      A dataset item can be a INPE Format PCD, TOA5 PCD, an occurrence of fire or an occurrence of diseases.

     */
    class DataSetItem : boost::noncopyable
    {
      friend class DataSetItemDAO;

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

          \param d  The dataset to which this item belongs to.
          \param k  The type of dataset item: PCD-INPE, PCD-TOA5, FIRE-POINTS, ...
          \param id The dataset item identifier or zero if it doesn't have a valid one.
        */
        DataSetItem(DataSetPtr d, Kind k, const uint64_t id = 0);

        /*!
          \brief Destructor.
        */
        virtual ~DataSetItem();

        /*!
          \brief It returns the identifier of the dataset item.

          \return The identifier of the dataset item.
        */
        uint64_t id() const;

        /*!
          \brief It returns the the kind of the dataset item.

          \return The kind of the dataset item.
        */
        Kind kind() const;

        /*!
          \brief It sets the the kind of the dataset item.

          \param k The kind of the dataset item.
        */
        void setKind(const Kind& k);

        /*!
          \brief It returns the the status of the dataset item.

          \return The status of the dataset item.
        */
        Status status() const;

        /*!
          \brief It sets the the status of the dataset item.

          \param s The status of the dataset item.
        */
        void setStatus(const Status s);

        /*!
          \brief It returns the mask of the dataset item.

          \return The mask of the dataset item.
        */
        std::string mask() const;

        /*!
          \brief It sets the mask of the dataset item.

          \param m The mask of the dataset item.
        */
        void setMask(const std::string& m);

        /*!
          \brief It returns the timezone of the dataset item.

          \return The timezone of the dataset item.
        */
        std::string timezone() const;

        /*!
          \brief It sets the timezone of the dataset item.

          \param tz The timezone of the dataset item.
        */
        void setTimezone(const std::string& tz);

        /*!
          \brief It returns the dataset to which this item belongs to.

          \return The the dataset.
        */
        DataSetPtr dataset() const;

        /*!
          \brief It returns the filter to be used when collecting this data item.

          \return The filter to used when collecting this data item.
        */
        FilterPtr filter() const;

        /*!
          \brief It sets the filter to be used when collecting data.

          \param f The filter to used when collecting data.
        */
        void setFilter(FilterPtr f);

        /*!
          \brief It returns the storage strategy metadata.

          \param The storage strategy metadata.
        */
        const std::map<std::string, std::string>& storageMetadata() const;
      
        /*!
          \brief It returns the storage strategy metadata.
         
          \param The storage strategy metadata.
         */
        std::map<std::string, std::string>& storageMetadata();

        /*!
          \brief It sets the storage strategy metadata.

          \param sm The storage strategy metadata.
        */
        void setStorageMetadata(const std::map<std::string, std::string>& sm);

      protected:

        /*!
          \brief It sets the identifier of the dataset item.

          \param The identifier of the dataset item.
        */
        void setId(uint64_t id);

      private:

        uint64_t id_;
        Status status_;
        DataSetPtr dataSet_;
        Kind kind_;
        std::string mask_;
        std::string timezone_;
        FilterPtr filter_;
        std::map<std::string, std::string> storageMetadata_;
    };

    typedef std::shared_ptr<DataSetItem> DataSetItemPtr;

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATASETITEM_HPP__

