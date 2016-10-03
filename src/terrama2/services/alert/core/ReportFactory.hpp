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
  \file terrama2/services/alert/core/ReportFactory.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_SERVICES_ALERT_CORE_REPORT_FACTORY_HPP__
#define __TERRAMA2_SERVICES_ALERT_CORE_REPORT_FACTORY_HPP__

#include "Report.hpp"
#include "Shared.hpp"

// TerraLib
#include <terralib/common/Singleton.h>

// STL
#include <functional>
#include <map>

namespace terrama2
{
  namespace services
  {
    namespace alert
    {
      namespace core
      {
        class ReportFactory : public te::common::Singleton<ReportFactory>
        {
          public:
            //! Report constructor function.
            typedef std::function<ReportPtr (std::map<std::string, std::string>)> FactoryFnctType;
            //! Register a new Report constructor associated with the ReportType.
            bool add(terrama2::services::alert::core::ReportType reportType, FactoryFnctType f);
            //! Remove the Report constructor associated with the ReportType.
            void remove(terrama2::services::alert::core::ReportType reportType);
            //! Check if there is a Report constructor associated with the ReportType.
            bool find(terrama2::services::alert::core::ReportType reportType);
            /*!
              \brief Creates a Report

              The Report is constructed based on the ReportType.

              \param reportMetadata Params to generate the report.
            */
            terrama2::services::alert::core::ReportPtr make(terrama2::services::alert::core::ReportType reportType, std::map<std::string, std::string> reportMetadata);

          protected:
            friend class te::common::Singleton<ReportFactory>;

            //! Default constructor.
            ReportFactory() = default;
            //! Default destructor
            virtual ~ReportFactory() = default;

            ReportFactory(const ReportFactory& other) = delete;
            ReportFactory(ReportFactory&& other) = delete;
            ReportFactory& operator=(const ReportFactory& other) = delete;
            ReportFactory& operator=(ReportFactory&& other) = delete;

          private:

            std::map<terrama2::services::alert::core::ReportType, FactoryFnctType> factoriesMap_;
        };
      } /* core */
    } /* alert */
  } /* services */
}

#endif // __TERRAMA2_SERVICES_ALERT_CORE_REPORT_FACTORY_HPP__
