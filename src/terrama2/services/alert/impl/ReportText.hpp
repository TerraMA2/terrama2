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
  \file terrama2/services/alert/impl/ReportText.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_SERVICES_ALERT_IMPL_REPORT_TEXT_HPP__
#define __TERRAMA2_SERVICES_ALERT_IMPL_REPORT_TEXT_HPP__

#include "../core/Report.hpp"

#include <sstream>

namespace terrama2
{
  namespace services
  {
    namespace alert
    {
      namespace core
      {
        class ReportText : public Report
        {
          public:
            ReportText(std::map<std::string, std::string> reportMetadata);

            virtual ~ReportText() = default;
            ReportText(const ReportText& other) = default;
            ReportText(ReportText&& other) = default;
            ReportText& operator=(const ReportText& other) = default;
            ReportText& operator=(ReportText&& other) = default;

          protected:
            std::string processInternal(AlertPtr alertPtr,
                                        terrama2::core::DataSetPtr dataset,
                                        std::shared_ptr<te::dt::TimeInstantTZ> alertTime,
                                        std::shared_ptr<te::da::DataSet> alertDataSet);
        };
      } /* core */
    } /* alert */
  } /* services */
}

#endif // __TERRAMA2_SERVICES_ALERT_IMPL_REPORT_TEXT_HPP__
