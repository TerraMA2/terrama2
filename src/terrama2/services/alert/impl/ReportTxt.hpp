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
  \file terrama2/services/alert/impl/ReportTxt.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_SERVICES_ALERT_IMPL_REPORTER_HPP__
#define __TERRAMA2_SERVICES_ALERT_IMPL_REPORTER_HPP__

#include "../core/Report.hpp"

namespace terrama2
{
  namespace services
  {
    namespace alert
    {
      namespace core
      {
        namespace ReportTags
        {
          const std::string FILE_NAME = "file_name";
          const std::string DESTINATION_FOLDER = "destination_folder";
        } /* ReportTags */

        class ReportTxt : public Report
        {
          public:
            ReportTxt(std::unordered_map<std::string, std::string> reportMetadata);

            virtual ~ReportTxt() = default;
            ReportTxt(const ReportTxt& other) = default;
            ReportTxt(ReportTxt&& other) = default;
            ReportTxt& operator=(const ReportTxt& other) = default;
            ReportTxt& operator=(ReportTxt&& other) = default;

            virtual void process(AlertPtr alertPtr,
                                 terrama2::core::DataSetPtr dataset,
                                 std::shared_ptr<te::dt::TimeInstantTZ> alertTime,
                                 std::shared_ptr<te::da::DataSet> alertDataSet) override;

            std::string refactorMask(const std::string& filePath, AlertPtr , std::shared_ptr<te::dt::TimeInstantTZ> alertTime) const;

            static ReportPtr make(std::unordered_map<std::string, std::string> reportMetadata);
            static ReportType reportType() { return "TXT"; }
        };
      } /* core */
    } /* alert */
  } /* services */
}

#endif // __TERRAMA2_SERVICES_ALERT_IMPL_REPORTER_HPP__
