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
  \file terrama2/core/Filter.hpp

  \brief Metadata about a given dataset item.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_CORE_FILTER_HPP__
#define __TERRAMA2_CORE_FILTER_HPP__

// STL
#include <memory>
#include <string>

// TerraLib
#include <terralib/datatype/DateTime.h>

// Forward declaration
namespace te
{
  namespace gm
  {
    class Geometry;
  }

  namespace dt
  {
    class DateTime;
  }
}


namespace terrama2
{
  namespace core
  {

    class DataSetItem;
    typedef std::shared_ptr<DataSetItem> DataSetItemPtr;


    /*!
      \class Filter

      \brief Contains the filters to be applied in a dataset item.

     */
    class Filter
    {

      public:


        //! Filter by value type.
        enum ByValueType
        {
          NONE_TYPE,
          LESS_THAN_TYPE,
          GREATER_THAN_TYPE,
          MEAN_LESS_THAN_TYPE,
          MEAN_GREATER_THAN_TYPE
        };

        /*!
          \brief Constructor
        */
        Filter(DataSetItemPtr dataSetItemPtr);

        /*!
          \brief Destructor
        */
        ~Filter();

        /*!
          \brief It returns the dataset item.

          \return The dataset item.
        */
        DataSetItemPtr dataSetItemPtr() const;

        /*!
          \brief It sets the dataset item.

          \param The dataset item.
        */
        void setDataSetItemPtr(const DataSetItemPtr& dataSetItemPtr);

        /*!
          \brief It returns Initial date of interest.

          \return Initial date of interest.
        */
        std::shared_ptr<te::dt::DateTime> discardBefore();

        /*!
          \brief It sets Initial date of interest.

          \param Initial date of interest.
        */
        void setDiscardBefore(std::shared_ptr<te::dt::DateTime> discardBefore);

        /*!
          \brief It returns the final date of interest.

          \return The final date of interest.
        */
        std::shared_ptr<te::dt::DateTime> discardAfter();

        /*!
          \brief It sets the final date of interest.

          \param The final date of interest.
        */
        void setDiscardAfter(std::shared_ptr<te::dt::DateTime> discardAfter);

        /*!
          \brief It returns the geometry to be used as area of interest.

          \return The geometry to be used as area of interest.
        */
        std::shared_ptr<te::gm::Geometry> geometry();

        /*!
          \brief It sets the geometry to be used as area of interest.

          \param The geometry to be used as area of interest.
        */
        void setGeometry(std::shared_ptr<te::gm::Geometry> geometry);

        /*!
          \brief It returns the value to be used in a filter by value.

          \return The value to be used in a filter by value.
        */
        double byValue() const;

        /*!
          \brief It sets the value to be used in a filter by value.

          \param The value to be used in a filter by value.
        */
        void setByValue(const double byValue);

        /*!
          \brief It returns the enum that defines the filter by value type.

          \return The enum that defines the filter by value type.
        */
        ByValueType byValueType() const;

        /*!
          \brief It sets the enum that defines the filter by value type.

          \param The enum that defines the filter by value type.
        */
        void setByValueType(const ByValueType byValueType);

        /*!
          \brief It returns the band filter.

          \return The band filter.
        */
        std::string bandFilter() const;

        /*!
          \brief It sets the band filter.

          \param The band filter.
        */
        void setBandFilter(const std::string& bandFilter);

      private:

        DataSetItemPtr dataSetItemPtr_;
        std::shared_ptr<te::dt::DateTime> discardBefore_;
        std::shared_ptr<te::dt::DateTime> discardAfter_;
        std::shared_ptr<te::gm::Geometry> geometry_;
        double byValue_;
        ByValueType byValueType_;
        std::string bandFilter_;


    };

    typedef std::shared_ptr<Filter> FilterPtr;

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_FILTER_HPP__

