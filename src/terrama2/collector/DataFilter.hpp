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
    /*!
     * \brief The DataFilter class is responsible for filtering file names and dataset data.

      DataFilter can filter:
        - Valid names (DataSetItem mask)
        - Dates (discardBefore_ and discardAfter_, last logged date)
        - Geometry (in development)

      File names will be filtered if they match the mask and dates in the valid date range.
      DataSet will be filtered by:
        - Date if the have a te::dt::DATETIME_TYPE column
        - Geometry if the have a te::dt::GEOMETRY_TYPE column

      The class will hold the latest date found for log.
     */
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
             * \brief Returns the lastest timestamp found in the DataSetItem.
             */
        te::dt::DateTime* getDataSetLastDateTime() const;

      private:
        //! Prepare mask data for wildcards identification
        void processMask();
        //! Returns true if the date is after discardBefore_ and before discardAfter. Updates dataSetLastDateTime_ with the latest date.
        bool validateDate(int dateColumn, const std::shared_ptr<te::da::DataSet> &dataSet);

        const core::DataSetItem& datasetItem_; //! DataSetItem to be filtered
        std::unique_ptr< te::dt::DateTime >  dataSetLastDateTime_; //! Latest valid date found
        std::shared_ptr<te::dt::DateTime> discardBefore_; //! Only date after this will be valid
        std::shared_ptr<te::dt::DateTime> discardAfter_;//! Only date before this will be valid

        struct
        {
          const std::string year4Str     = "%A"; //! Wilcard string for 4 digit year
          const std::string year2Str     = "%a"; //! Wilcard string for 2 digit year
          const std::string monthStr     = "%M"; //! Wilcard string for 2 digit month
          const std::string dayStr       = "%d"; //! Wilcard string for 2 digit day
          const std::string hourStr      = "%h"; //! Wilcard string for 2 digit hour
          const std::string minuteStr    = "%m"; //! Wilcard string for 2 digit minute
          const std::string secondStr    = "%s"; //! Wilcard string for 2 digit second
          const std::string wildCharStr  = "%."; //! Wilcard string for any digit


          //! \cond PRIVATE
          //
          // position of wildcards in the mask, corrected by real length
          int year4Pos     = -1;
          int year2Pos     = -1;
          int monthPos     = -1;
          int dayPos       = -1;
          int hourPos      = -1;
          int minutePos    = -1;
          int secondPos    = -1;
          int wildCharPos  = -1;
          //! \endcond

          boost::regex regex; //! Regex of mask
        } maskData;//! Preprocessed mask data to validade filenames
    };

    typedef std::shared_ptr<DataFilter> DataFilterPtr;
  }
}


#endif //__TERRAMA2_COLLECTOR_FILTER_HPP__
