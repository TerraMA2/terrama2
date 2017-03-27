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
  \file terrama2/services/alert/impl/NotifierEmail.hpp

  \brief

  \author Jano Simas
          Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_ALERT_IMPL_EMAIL_NOTIFIER_HPP__
#define __TERRAMA2_SERVICES_ALERT_IMPL_EMAIL_NOTIFIER_HPP__

// TerraMA2
#include "../core/Notifier.hpp"
#include "../core/Shared.hpp"

// VMime
#include <vmime/vmime.hpp>

// STL
#include <string>

namespace terrama2
{
  namespace services
  {
    namespace alert
    {
      namespace impl
      {
        class NotifierEmail : public core::Notifier
        {
          public:
            NotifierEmail(const std::map<std::string, std::string>& serverMap, core::ReportPtr report);

            ~NotifierEmail() = default;
            NotifierEmail(const NotifierEmail& other) = default;
            NotifierEmail(NotifierEmail&& other) = default;
            NotifierEmail& operator=(const NotifierEmail& other) = default;
            NotifierEmail& operator=(NotifierEmail&& other) = default;

            virtual void send(const std::vector<std::string>& recipient, int riskLevel, bool notifyOnChange) const;

            static std::string notifierCode() { return "EMAIL"; };

            static core::NotifierPtr make(const std::map<std::string, std::string>& serverMap, core::ReportPtr report)
            {
              return std::make_shared<NotifierEmail>(serverMap,report);
            }


          protected:

            static vmime::shared_ptr <vmime::net::session> session_;

        };
      } /* impl */
    } /* alert */
  } /* services */
} /* terrama2 */

#endif //__TERRAMA2_SERVICES_ALERT_IMPL_EMAIL_NOTIFIER_HPP__
