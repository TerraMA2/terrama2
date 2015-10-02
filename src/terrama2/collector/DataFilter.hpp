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
  \file terrama2/collector/Filter.hpp

  \brief Filters data.

  \author Jano Simas
*/


#ifndef __TERRAMA2_COLLECTOR_FILTER_HPP__
#define __TERRAMA2_COLLECTOR_FILTER_HPP__

#include "../core/DataSetItem.hpp"

//Terralib
#include "terralib/dataaccess/dataset/DataSet.h"
#include "terralib/geometry.h"

//STD
#include <string>
#include <vector>

//Boost
#include <boost/noncopyable.hpp>

namespace terrama2
{
  namespace collector
  {
    class DataFilter : public boost::noncopyable
    {
      public:
        DataFilter(core::DataSetItemPtr datasetItem);
        ~DataFilter();

        /*!
             * \brief Filters a list of names using filtering criteria.
             *
             * \param namesList Full list of names to be filtered.
             *
             * \pre Filtering rules should have been set, otherwise unmodified list will be returned.
             *
             * \return List of filtered names.
             */
        std::vector<std::string> filterNames(const std::vector<std::string> &namesList) const;

        /*!
             * \brief Filters a te::da::DataSet by matching criteria.
             * \param dataSet DataSet to be filtered.
             *
             * \pre Filtering rules should have been set, otherwise unmodified DataSet is returned.
             *
             * \return Filtered DataSet.
             */
        std::shared_ptr<te::da::DataSet> filterDataSet(const std::shared_ptr<te::da::DataSet> &dataSet) const;

        //TODO: should have static methods for easy access?
        static std::vector<std::string> filterNamesByMask(const std::vector<std::string>& namesList, const std::string& mask);
        static te::da::DataSetPtr filterDataSetByIntersection(const te::da::DataSetPtr dataset, const te::gm::GeometryShrPtr geometry);

      private:

        /*!
             * \brief Sets the mask the names should match
             * .
             * //TODO: define the wild cards to mask may have
             *
             */
        void setMask(const std::string& mask);

        /*!
             * \brief Sets the geometry and the relation rule the data must comply to be accepted
             * \param geometry Geometry to be confronted with the data.
             * \param relationRule Relation rule the data must comply.
             *
             * //TODO: enum the possible relations
             */
        void setGeometry(const te::gm::GeometryShrPtr geometry, te::gm::SpatialRelation relationRule);

        /*!
             * \brief Sets the date/time that should be used as starting date/time of valid data.
             * \param startDateTime Starting valid data date/time.
             */
        void setDataStartDate();//TODO: What datetime format?
        /*!
             * \brief Sets the date/time that should be used as last date/time of valid data.
             * \param endDateTime Last valid data date/time.
             */
        void setDataEndDate();//TODO: What datetime format?

        struct Impl;
        Impl* impl_;
    };

    typedef std::shared_ptr<DataFilter> DataFilterPtr;
  }
}


#endif //__TERRAMA2_COLLECTOR_FILTER_HPP__
