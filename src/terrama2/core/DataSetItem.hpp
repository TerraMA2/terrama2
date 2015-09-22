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


namespace terrama2
{
  namespace core
  {
    class DataSet;
    typedef std::shared_ptr<DataSet> DataSetPtr;

    class Filter;
    typedef std::shared_ptr<Filter> FilterPtr;

    /*!
      \class DataSetItem

      \brief Contains metadata about a dataset item.

      A dataset item can be a INPE Format PCD, TOA5 PCD, an occurrence of fire or an occurrence of diseases.

     */
    class DataSetItem
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
        */
        DataSetItem(DataSetPtr dataSet, Kind kind, const uint64_t id = 0);

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

          \param The kind of the dataset item.
        */
        void setKind(const Kind& kind);

        /*!
          \brief It returns the the status of the dataset item.

          \return The status of the dataset item.
        */
        Status status() const;

        /*!
          \brief It sets the the status of the dataset item.

          \param The status of the dataset item.
        */
        void setStatus(const Status& status);

        /*!
          \brief It returns the mask of the dataset item.

          \return The mask of the dataset item.
        */
        std::string mask() const;

        /*!
          \brief It sets the mask of the dataset item.

          \param The mask of the dataset item.
        */
        void setMask(const std::string& mask);

        /*!
          \brief It returns the timezone of the dataset item.

          \return The timezone of the dataset item.
        */
        std::string timezone() const;

        /*!
          \brief It sets the timezone of the dataset item.

          \param The timezone of the dataset item.
        */
        void setTimezone(const std::string& timezone);

        /*!
          \brief It returns the the dataset.

          \return The the dataset.
        */
        DataSetPtr dataSet() const;

        /*!
          \brief It returns the filter to be used when collecting data.

          \return The filter to used when collecting data.
        */
        FilterPtr filter() const;

        /*!
          \brief It sets the filter to be used when collecting data.

          \param The filter to used when collecting data.
        */
        void setFilter(FilterPtr filter);


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

      friend class DataSetDAO;
    };

    typedef std::shared_ptr<DataSetItem> DataSetItemPtr;

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_DATASETITEM_HPP__

