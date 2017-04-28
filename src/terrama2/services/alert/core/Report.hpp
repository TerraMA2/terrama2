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
  \file terrama2/services/alert/core/Report.hpp

  \brief

  \author Jano Simas
          Vinicius Campanha
 */

#ifndef __TERRAMA2_SERVICES_ALERT_CORE_REPORT_HPP__
#define __TERRAMA2_SERVICES_ALERT_CORE_REPORT_HPP__

// TerraMA2
#include "Shared.hpp"
#include "../../../core/Shared.hpp"
#include "../../../core/data-model/DataSeriesSemantics.hpp"
#include "Alert.hpp"

// TerraLib
#include <terralib/dataaccess/dataset/DataSetType.h>
#include <terralib/dataaccess/dataset/FilteredDataSet.h>
#include <terralib/memory/DataSet.h>

// STL
#include <string>
#include <unordered_map>



namespace te
{
  namespace dt
  {
    class TimeInstantTZ;
  } /* dt */
  namespace da
  {
    class DataSet;
  } /* da */
} /* te */

namespace terrama2
{
  namespace services
  {
    namespace alert
    {
      namespace core
      {
        typedef std::string ReportType;

        namespace ReportTags
        {
          const std::string TITLE = "title";
          const std::string ABSTRACT = "abstract";
          const std::string DESCRIPTION = "description";
          const std::string AUTHOR = "author";
          const std::string CONTACT = "contact";
          const std::string COPYRIGHT = "copyright";
          const std::string LOGO_PATH = "logo_path";
          const std::string TIMESTAMP_FORMAT = "timestamp_format";
        } /* ReportTags */

        class Report
        {
          public:
            Report(AlertPtr alert,
                   terrama2::core::DataSeriesPtr alertDataSeries,
                   std::shared_ptr<te::da::DataSet> alertDataSet,
                   std::vector<std::shared_ptr<te::dt::DateTime>> riskDates);

            ~Report() = default;
            Report(const Report& other) = default;
            Report(Report&& other) = default;
            Report& operator=(const Report& other) = default;
            Report& operator=(Report&& other) = default;

            //*! Gets the report title
            std::string title() const { return alert_->reportMetadata.at(ReportTags::TITLE); }

            //! Gets the report abstract
            std::string abstract() const { return alert_->reportMetadata.at(ReportTags::ABSTRACT); }

            //! Gets the report description
            std::string description() const { return alert_->reportMetadata.at(ReportTags::DESCRIPTION); }

            //! Gets the report author
            std::string author() const { return alert_->reportMetadata.at(ReportTags::AUTHOR); }

            //! Gets the report contact
            std::string contact() const { return alert_->reportMetadata.at(ReportTags::CONTACT); }

            //! Gets the report copyright
            std::string copyright() const { return alert_->reportMetadata.at(ReportTags::COPYRIGHT); }

            //! Gets the logo path to be used in the report
            std::string logoPath() const { return alert_->reportMetadata.at(ReportTags::LOGO_PATH); }

            //! Gets the timestamp format to be used in the report
            std::string timeStampFormat() const { return alert_->reportMetadata.at(ReportTags::TIMESTAMP_FORMAT); }

            //! Gets the save path to save a document report
            std::string documentSavePath() const;

            /*!
             * \brief Returns a dataSet with all data
             * \return A dataSet with all data
             */
            std::shared_ptr<te::da::DataSet> retrieveAllData() const;

            /*!
             * \brief Gets a DataSet based in the values of the comparison property
             * \param The list of wanted values
             * \return A DataSet with only the data that the comparison property is in values list
             */
            std::shared_ptr<te::da::DataSet> retrieveDataComparisonValue(const std::vector<int>& values) const;

            /*!
             * \brief Returns a DataSet with the data that has changed their risk value
             * \return A DataSet with the data that has changed their risk value
             */
            std::shared_ptr<te::da::DataSet> retrieveDataChangedRisk() const;

            /*!
             * \brief Returns a DataSet with the data that has not changed their risk value
             * \return A DataSet with the data that has not changed their risk value
             */
            std::shared_ptr<te::da::DataSet> retrieveDataUnchangedRisk() const;

            /*!
             * \brief Returns a DataSet with the data that their risk value has increased
             * \return A DataSet with the data that their risk value has increased
             */
            std::shared_ptr<te::da::DataSet> retrieveDataIncreasedRisk() const;

            /*!
             * \brief Returns a DataSet with the data that their risk value has decreased
             * \return A DataSet with the data that their risk value has decreased
             */
            std::shared_ptr<te::da::DataSet> retrieveDataDecreasedRisk() const;

            /*!
             * \brief Returns a DataSet that the data is at a specifc risk value
             * \param The risk value wanted
             * \return A DataSet that the data is at a specifc risk value
             */
            std::shared_ptr<te::da::DataSet> retrieveDataAtRisk(const int risk) const;

            /*!
             * \brief Returns a DataSet that the data risk value is equal or above (>=) a specifc value
             * \param The risk value
             * \return A DataSet that the data risk value is equal or above (>=) a specifc value
             */
            std::shared_ptr<te::da::DataSet> retrieveDataAboveRisk(const int risk) const;

            /*!
             * \brief Returns a DataSet that the data risk value is equal or below (<=) a specifc value
             * \param The risk value
             * \return A DataSet that the data risk value is equal or below (<=) a specifc value
             */
            std::shared_ptr<te::da::DataSet> retrieveDataBelowRisk(const int risk) const;

            double retrieveMaxValue() const;

            double retrieveMinValue() const;

            double retrieveMeanValue() const;

            terrama2::core::DataSeriesType dataSeriesType() const;

          protected:

            /*!
             * \brief Sets the report DataSet and adjust it to reports
             * \param The DataSet
             */
            void updateReportDataset(const std::shared_ptr<te::da::DataSet> dataSet);

          protected:

            AlertPtr alert_; //!< The alert information
            terrama2::core::DataSeriesPtr alertDataSeries_;
            std::shared_ptr<te::mem::DataSet> dataSet_; //!< The dataSet with alert data
            std::vector<std::shared_ptr<te::dt::DateTime>> riskDates_; //!< A list with the datetime of each risk calculation

        };
      } /* core */
    } /* alert */
  } /* services */
}

#endif // __TERRAMA2_SERVICES_ALERT_CORE_REPORT_HPP__
