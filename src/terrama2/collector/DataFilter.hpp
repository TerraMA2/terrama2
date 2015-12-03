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
  \author Evandro Delatin
*/


#ifndef __TERRAMA2_COLLECTOR_FILTER_HPP__
#define __TERRAMA2_COLLECTOR_FILTER_HPP__

#include "../core/DataSetItem.hpp"

//Terralib
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/datatype/TimeDuration.h>
#include <terralib/geometry.h>

//STD
#include <string>
#include <vector>

//Boost
#include <boost/noncopyable.hpp>
#include <boost/regex.hpp>

namespace terrama2
{
  namespace collector
  {
    class DataFilter : public boost::noncopyable
    {
      public:
        DataFilter(const core::DataSetItem& datasetItem);
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
        std::shared_ptr<te::da::DataSet> filterDataSet(const std::shared_ptr<te::da::DataSet> &dataSet, const std::shared_ptr<te::da::DataSetType>& datasetType);

        /*!
             * \brief Returns the last timestamp that the DataSetItem has.
             *
             * \pre
             *
             * \return DateTime with the last timestamp from DataSetItem.
             */
        te::dt::DateTime* getDataSetLastDateTime() const;

      private:
        //! Prepare mask data for wildcards identification
        void processMask();

        const core::DataSetItem& datasetItem_;
        std::unique_ptr< te::dt::DateTime >  dataSetLastDateTime_;
        std::shared_ptr<te::dt::DateTime> discardBefore;
        std::shared_ptr<te::dt::DateTime> discardAfter;

        struct
        {
          const std::string year4Str     = "%A";
          const std::string year2Str     = "%a";
          const std::string monthStr     = "%M";
          const std::string dayStr       = "%d";
          const std::string hourStr      = "%h";
          const std::string minuteStr    = "%m";
          const std::string secondStr    = "%s";
          const std::string wildCharStr  = "%.";

          int year4Pos     = -1;
          int year2Pos     = -1;
          int monthPos     = -1;
          int dayPos       = -1;
          int hourPos      = -1;
          int minutePos    = -1;
          int secondPos    = -1;
          int wildCharPos  = -1;

          boost::regex regex;
        } maskData;
    };

    typedef std::shared_ptr<DataFilter> DataFilterPtr;
  }
}


#endif //__TERRAMA2_COLLECTOR_FILTER_HPP__
