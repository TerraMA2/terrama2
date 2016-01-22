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

  \brief The DataFilter class is responsible for filtering file names and data.

  \author Jano Simas
  \author Evandro Delatin
*/


#ifndef __TERRAMA2_COLLECTOR_FILTER_HPP__
#define __TERRAMA2_COLLECTOR_FILTER_HPP__

#include "Log.hpp"
#include "TransferenceData.hpp"
#include "../core/DataSetItem.hpp"
#include "../core/DataSet.hpp"

//Terralib
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/datatype/Date.h>
#include <terralib/datatype/TimeInstant.h>
#include <terralib/datatype/TimeInstantTZ.h>
#include <terralib/datatype/TimeDuration.h>
#include <terralib/geometry.h>

//STL
#include <string>
#include <vector>

//Boost
#include <boost/date_time/local_time/local_date_time.hpp>
#include <boost/noncopyable.hpp>
#include <boost/regex.hpp>

namespace te
{
  namespace dt
  {
    class TimeInstantTZ;
  }
}

namespace terrama2
{
  namespace collector
  {
    /*!
     * \brief The DataFilter class is responsible for filtering file names and data.

      DataFilter can filter:
        - Valid names (core::DataSetItem mask)
        - Dates (discardBefore_ and discardAfter_, last logged date)
        - Geometry (in development)

      File names will be filtered if they match the mask and dates in the valid date range.
      core::DataSet will be filtered by:
        - Date if the have a te::dt::DATETIME_TYPE column
        - Geometry if the have a te::dt::GEOMETRY_TYPE column

      The class will hold the latest date found for log.
     */
    class DataFilter : public boost::noncopyable
    {
    public:
      /*!
           \brief Creates a DataFilter object to filter core::DataSetItem data.

           Recover last collected date for the core::DataSetItem,
           and pre process the mask.


           \exception EmptyMaskError Raise when core::DataSetItem Filter mask is not set.
         */
      DataFilter(const core::DataSetItem& datasetItem, std::shared_ptr<te::dt::TimeInstantTZ> lastLogTime = nullptr);
      ~DataFilter();

      /*!
              \brief Filters a list of names by matching criteria.

              \note Updates dataSetLastDateTime_

              \param namesList Full list of names to be filtered.

              \return List of filtered names.
             */
      std::vector<std::string> filterNames(const std::vector<std::string> &namesList);
      bool filterName(const std::string &name);

      /*!
             \brief Filters a te::da::DataSet by matching criteria.

             Will only filter dates if there is a te::dt::DATETIME_TYPE attribute in the dataset,
             geometry expects a te::dt::GEOMETRY_TYPE attribute.

             \note geometry filter is not implemented yet

             \note Updates dataSetLastDateTime_

             \param transferenceData Data of the DataSet to be filtered.

             \pre Filtering rules should have been set, otherwise unmodified DataSet is returned.

             \pre Terralib should be initialized.
             */
      void filterDataSet(terrama2::collector::TransferenceData& transferenceData);

    private:
      //! Updates last date time collected.
      void updateLastDateTimeCollected(boost::local_time::local_date_time boostTime);

      /*!
           \brief Prepare mask data for wildcards identification

           \exception EmptyMaskError Raise when core::DataSetItem Filter mask is not set.
         */
      void processMask();
      /*!
         \brief Returns true if the date is after discardBefore_ and before discardAfter.

         \warning Updates dataSetLastDateTime_ with the latest date.
         */
      bool validateAndUpdateDate(int dateColumn, const std::shared_ptr<te::da::DataSet> &dataSet, TransferenceData& transferenceData);

      bool validateGeometry(int geometryColumn, const std::shared_ptr<te::da::DataSet> &dataSet);

      /*!
           \brief Verifies if the time is after discardBeforeTime.

           Check if hours, minutes and seconds are after discardBeforeTime,
           if any of them is -1 it's considered after, the others are checked.
         */
      bool isAfterDiscardBeforeTime(int hours, int minutes, int seconds, const boost::posix_time::time_duration& discardBeforeTime) const;
      /*!
           \brief Verifies if the time is after discarBeforeDate.

           Check if year, month and day are after discarBeforeDate,
           if any of them is 0 it's considered after, the others are checked.
         */
      bool isAfterDiscardBeforeDate(unsigned int year, unsigned int month, unsigned int day, const boost::gregorian::date& discarBeforeDate) const;
      //! Check if value == 0 or value >= discardBeforeValue
      bool isAfterDiscardBeforeValue(unsigned int value, unsigned int discardBeforeValue) const;


      /*!
           \brief Verifies if the time is before discardAfterTime.

           Check if hours, minutes and seconds are before discardAfterTime,
           if any of them is -1 it's considered before, the others are checked.
         */
      bool isBeforeDiscardAfterTime(int hours, int minutes, int seconds, const boost::posix_time::time_duration& discardAfterTime) const;
      /*!
           \brief Verifies if the time is before discardAfterDate.

           Check if year, month and day are before discardAfterDate,
           if any of them is 0 it's considered before, the others are checked.
         */
      bool isBeforeDiscardAfterDate(unsigned int year, unsigned int month, unsigned int day, const boost::gregorian::date& discardAfterDate) const;
      //! Check if value == 0 or value <= discardAfterValue
      bool isBeforeDiscardAfterValue(unsigned int value, unsigned int discardAfterValue) const;

      const core::DataSetItem& datasetItem_; //!< core::DataSetItem to be filtered
      std::unique_ptr<const te::gm::Geometry> geometry_;
      std::shared_ptr<te::dt::TimeInstantTZ>  discardBefore_; //!< Earliest valid time/date.
      std::shared_ptr<te::dt::TimeInstantTZ>  discardAfter_; //!< Latest valid time/date.
      terrama2::collector::TransferenceData*  currentData_;

      struct
      {
        const std::string year4Str     = "%A"; //!< Wilcard string for 4 digit year
        const std::string year2Str     = "%a"; //!< Wilcard string for 2 digit year
        const std::string monthStr     = "%M"; //!< Wilcard string for 2 digit month
        const std::string dayStr       = "%d"; //!< Wilcard string for 2 digit day
        const std::string hourStr      = "%h"; //!< Wilcard string for 2 digit hour
        const std::string minuteStr    = "%m"; //!< Wilcard string for 2 digit minute
        const std::string secondStr    = "%s"; //!< Wilcard string for 2 digit second
        const std::string wildCharStr  = "%."; //!< Wilcard string for any digit

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

        boost::regex regex; //!< Regex of mask
      } maskData;//!< Preprocessed mask data to validade filenames
    };

    typedef std::shared_ptr<DataFilter> DataFilterPtr; //!< Shared pointer for DataFilter.
  }
}


#endif //__TERRAMA2_COLLECTOR_FILTER_HPP__
