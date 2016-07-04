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
 */

#ifndef __TERRAMA2_SERVICES_ALERT_CORE_REPORTER_HPP__
#define __TERRAMA2_SERVICES_ALERT_CORE_REPORTER_HPP__

#include <string>
#include <unordered_map>

#include "Shared.hpp"
#include "../../../core/Shared.hpp"

namespace te {
  namespace dt {
    class TimeInstantTZ;
  } /* dt */
  namespace da {
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
          const std::string TYPE = "type";
          const std::string TITLE = "title";
          const std::string SUBTITLE = "subtitle";
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
            Report(std::unordered_map<std::string, std::string> reportMetadata)
              : reportMetadata_(reportMetadata) {}

            ~Report() = default;
            Report(const Report& other) = default;
            Report(Report&& other) = default;
            Report& operator=(const Report& other) = default;
            Report& operator=(Report&& other) = default;

            virtual void process(AlertPtr alertPtr,
                                 terrama2::core::DataSetPtr dataset,
                                 std::shared_ptr<te::dt::TimeInstantTZ> alertTime,
                                 std::shared_ptr<te::da::DataSet> alertDataSet) = 0;

          protected:
            std::unordered_map<std::string, std::string> reportMetadata_;
        };
      } /* core */
    } /* alert */
  } /* services */
}

#endif // __TERRAMA2_SERVICES_ALERT_CORE_REPORTER_HPP__
