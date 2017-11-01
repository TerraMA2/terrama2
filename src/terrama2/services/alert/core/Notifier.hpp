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
  \file terrama2/services/alert/core/Notifier.hpp

  \brief

  \author Jano Simas
          Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_ALERT_CORE_NOTIFIER_HPP__
#define __TERRAMA2_SERVICES_ALERT_CORE_NOTIFIER_HPP__


// TerraMA2
#include "Shared.hpp"
#include "Alert.hpp"

#include <terralib/core/uri/URI.h>

// STL
#include <map>
#include <vector>



namespace terrama2
{
  namespace services
  {
    namespace alert
    {
      namespace core
      {
        class Notifier
        {
        public:
          Notifier(const std::map<std::string, std::string>& serverMap, ReportPtr report)
                       : serverMap_(serverMap),
                         report_(report) { }

          ~Notifier() = default;
          Notifier(const Notifier& other) = default;
          Notifier(Notifier&& other) = default;
          Notifier& operator=(const Notifier& other) = default;
          Notifier& operator=(Notifier&& other) = default;

          virtual void send(const Notification& recipient,
                            const te::core::URI& documentURI) const = 0;

        protected:
          const std::map<std::string, std::string> serverMap_;
          ReportPtr report_;

          int highestRiskLevel = 0;
          bool riskChanged = false;
        };
      } /* core */
    } /* alert */
  } /* services */
} /* terrama2 */

#endif //__TERRAMA2_SERVICES_ALERT_CORE_NOTIFIER_HPP__
