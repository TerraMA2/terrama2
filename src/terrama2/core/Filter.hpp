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

  \brief Filter information of a given dataset item.

  \author Paulo R. M. Oliveira
  \author Gilberto Ribeiro de Queiroz
*/

#ifndef __TERRAMA2_CORE_FILTER_HPP__
#define __TERRAMA2_CORE_FILTER_HPP__

// STL
#include <memory>
#include <string>

// Boost
#include <boost/noncopyable.hpp>

// Forward declaration
namespace te
{
  namespace dt { class DateTime; }
  namespace gm { class Geometry; }
}


namespace terrama2
{
  namespace core
  {
// Forward declaration
    class DataSetItem;

    /*!
      \class Filter

      \brief Filter information of a given dataset item.
     */
    class Filter : public boost::noncopyable
    {

      public:

        //! Filter by value type.
        enum ExpressionType
        {
          NONE_TYPE,
          LESS_THAN_TYPE,
          GREATER_THAN_TYPE,
          MEAN_LESS_THAN_TYPE,
          MEAN_GREATER_THAN_TYPE
        };

        /*!
          \brief Constructor.

          \param item The associated dataset item.
        */
        Filter(const DataSetItem* item = nullptr);

        /*! \brief Destructor. */
        ~Filter();

        /*! \brief Returns a pointer to the associated dataset item. */
        const DataSetItem* datasetItem() const;

        /*!
          \brief Associates the filter to given dataset item.

          \param item The dataset item to be associated to this filter.
        */
        void setDataSetItem(const DataSetItem* item);

        /*! \brief Returns the initial date of interest for collecting data from the data item. */
        const te::dt::DateTime* discardBefore() const;

        /*! \brief Sets the initial date of interest for collecting data from the data item. */
        void setDiscardBefore(std::unique_ptr<te::dt::DateTime> t);

        /*! \brief Returns the final date of interest for collecting data from the data item. */
        const te::dt::DateTime* discardAfter() const;

        /*! \brief Sets the final date of interest for collecting data from the data item. */
        void setDiscardAfter(std::unique_ptr<te::dt::DateTime> t);

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

      private:

        DataSetItemPtr datasetItem_;
        std::unique_ptr<te::dt::DateTime> discardBefore_;
        std::unique_ptr<te::dt::DateTime> discardAfter_;
        std::unique_ptr<te::gm::Geometry> geometry_;
        std::unique_ptr<double> value_;
        ExpressionType expressionType_;
        std::string bandFilter_;
    };

    typedef std::shared_ptr<Filter> FilterPtr;

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_FILTER_HPP__

