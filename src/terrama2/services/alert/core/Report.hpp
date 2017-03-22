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
                   std::shared_ptr<te::da::DataSet> alertDataSet,
                   std::shared_ptr<te::da::DataSetType> alertDataSetType,
                   std::vector<std::shared_ptr<te::dt::DateTime>> riskDates);

            ~Report() = default;
            Report(const Report& other) = default;
            Report(Report&& other) = default;
            Report& operator=(const Report& other) = default;
            Report& operator=(Report&& other) = default;

            std::string title() const { return alert_->reportMetadata.at(ReportTags::TITLE); }

            std::string abstract() const { return alert_->reportMetadata.at(ReportTags::ABSTRACT); }

            std::string description() const { return alert_->reportMetadata.at(ReportTags::DESCRIPTION); }

            std::string author() const { return alert_->reportMetadata.at(ReportTags::AUTHOR); }

            std::string contact() const { return alert_->reportMetadata.at(ReportTags::CONTACT); }

            std::string copyright() const { return alert_->reportMetadata.at(ReportTags::COPYRIGHT); }

            std::string logoPath() const { return alert_->reportMetadata.at(ReportTags::LOGO_PATH); }

            std::string timeStampFormat() const { return alert_->reportMetadata.at(ReportTags::TIMESTAMP_FORMAT); }

            std::shared_ptr<te::da::DataSet> retrieveData() const;

            std::shared_ptr<te::da::DataSet> retrieveDataComparisonValue(const std::vector<int>& values) const;

            std::shared_ptr<te::da::DataSet> retrieveDataChangedRisk() const;

            std::shared_ptr<te::da::DataSet> retrieveDataUnchangedRisk() const;

            std::shared_ptr<te::da::DataSet> retrieveDataIncreasedRisk() const;

            std::shared_ptr<te::da::DataSet> retrieveDataDecreasedRisk() const;

            std::shared_ptr<te::da::DataSet> retrieveDataAtRisk(const int risk) const;

            std::shared_ptr<te::da::DataSet> retrieveDataAboveRisk(const int risk) const;

            std::shared_ptr<te::da::DataSet> retrieveDataBelowRisk(const int risk) const;

          protected:

            void updateReportDataset(const std::shared_ptr<te::da::DataSet> dataSet,
                                     const std::shared_ptr<te::da::DataSetType> alertDataSetType) const;

            AlertPtr alert_;
            std::shared_ptr<te::mem::DataSet> dataSet_;
            std::shared_ptr<te::da::DataSetType> alertDataSetType_;
            std::vector<std::shared_ptr<te::dt::DateTime>> riskDates_;

        };
      } /* core */
    } /* alert */
  } /* services */
}

#endif // __TERRAMA2_SERVICES_ALERT_CORE_REPORT_HPP__
