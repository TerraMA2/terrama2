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

  \brief Filter information of a given DataSetItem.

  \author Paulo R. M. Oliveira
  \author Gilberto Ribeiro de Queiroz
*/

#ifndef __TERRAMA2_CORE_FILTER_HPP__
#define __TERRAMA2_CORE_FILTER_HPP__

// STL
#include <string>
#include <memory>

// Qt
#include <QJsonObject>

// Forward declaration
namespace te
{
  namespace dt { class TimeInstantTZ; }
  namespace gm { class Geometry; }
}


namespace terrama2
{
  namespace core
  {

    /*!
      \class Filter

      \brief Filter information of a given DataSetItem.
     */
    class Filter
    {

      public:

        /*! \brief Filter by value type.
         Each constant must exist in table terrama2.filter_expression_type and the value must be the same from column id.
         */
        enum ExpressionType
        {
          NONE_TYPE = 1, //!< No filter.
          LESS_THAN_TYPE = 2, //!< Eliminate data when all values are less than a given value
          GREATER_THAN_TYPE = 3, //!< Eliminate data when all values are greater than a given value
          MEAN_LESS_THAN_TYPE = 4, //!< Eliminate data when the mean is less than a given value
          MEAN_GREATER_THAN_TYPE = 5 //!< Eliminate data when the mean is greater than a given value
        };

        /*!
          \brief Constructor.

          \param item The associated DataSetItem.
        */
        Filter(uint64_t dataSetItemId = 0);

        /*! \brief Destructor. */
        ~Filter();


        /*! Copy constructor */
        Filter(const Filter& filter);

        /*! Assignment operator */
        Filter& operator=(const Filter& filter);

        /*! \brief Returns the identifier of the associated DataSetItem. */
        uint64_t datasetItem() const;


        /*!
          \brief Associates the filter to a given DataSetItem.

          \param item The DataSetItem to be associated to this filter.
        */
        void setDataSetItem(uint64_t item);

        /*! \brief Returns the initial date of interest for collecting data from the DataSetItem. */
        const te::dt::TimeInstantTZ* discardBefore() const;

        /*! \brief Sets the initial date of interest for collecting data from the DataSetItem. */
        void setDiscardBefore(std::unique_ptr<te::dt::TimeInstantTZ> t);

        /*! \brief Returns the final date of interest for collecting data from the DataSetItem. */
        const te::dt::TimeInstantTZ* discardAfter() const;

        /*! \brief Sets the final date of interest for collecting data from the DataSetItem. */
        void setDiscardAfter(std::unique_ptr<te::dt::TimeInstantTZ> t);

        /*! \brief Returns the geometry to be used as area of interest for filtering the data during its collect. */
        const te::gm::Geometry* geometry() const;

        /*! Sets the geometry to be used as area of interest for filtering the data during its collect. */
        void setGeometry(std::unique_ptr<te::gm::Geometry> geom);

        /*! \brief Returns the value to be used in a filter by value. */
        const double* value() const;

        /*! \brief Sets the value to be used in a filter by value. */
        void setValue(std::unique_ptr<double> v);

        /*! \brief Returns the type of filter by expression. */
        ExpressionType expressionType() const;

        /*! \brief Sets the type of filter by expression. */
        void setExpressionType(const ExpressionType t);

        /*! \brief Returns the band filter. */
        const std::string& bandFilter() const;

        /*! \brief Sets the band filter. */
        void setBandFilter(const std::string& f);

        /*! \brief Returns the identifier of the static data to use as area of interest. */
        uint64_t staticDataId() const;

        /*! \brief Sets the identifier of the static data to use as area of interest. */
        void setStaticDataId(const uint64_t staticDataId);

        /*! \brief Creates the object from the JSON string. */
        static Filter FromJson(QJsonObject json);

        /*! \brief Serialize to JSON. */
        QJsonObject toJson();

        /*! \brief Override operator == */
        bool operator==(const Filter& rhs);

        /*! \brief Override operator != */
        bool operator!=(const Filter& rhs);

      private:

        uint64_t datasetItem_; //!< Associates the filter to a given DataSetItem.
        std::unique_ptr<te::dt::TimeInstantTZ> discardBefore_; //!< Initial date of interest for collecting data from the DataSetItem.
        std::unique_ptr<te::dt::TimeInstantTZ> discardAfter_; //!< Final date of interest for collecting data from the DataSetItem.
        std::unique_ptr<te::gm::Geometry> geometry_; //!< Geometry to be used as area of interest for filtering the data during its collect.
        std::unique_ptr<double> value_; //!< Value to be used in a filter by value.
        ExpressionType expressionType_; //!< Type of filter by expression.
        std::string bandFilter_; //!< Band filter.
        uint64_t staticDataId_; //!< Identifier of the static data to use as area of interest
    };

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_FILTER_HPP__

