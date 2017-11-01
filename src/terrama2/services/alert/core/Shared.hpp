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
  \file terrama2/services/alert/core/Service.hpp

  \author Jano Simas
*/

#ifndef __TERRAMA2_SERVICES_ALERT_SHARED_HPP__
#define __TERRAMA2_SERVICES_ALERT_SHARED_HPP__

#include <memory>

namespace terrama2
{
  namespace services
  {
    namespace alert
    {
      namespace core
      {

        struct Alert;
        //! Shared smart pointer for Alert
        typedef std::shared_ptr<const terrama2::services::alert::core::Alert> AlertPtr;

        class Report;
        //! Shared smart pointer for Report
        typedef std::shared_ptr<terrama2::services::alert::core::Report> ReportPtr;

        class Notifier;
        //! Shared smart pointer for Notifier
        typedef std::shared_ptr<const terrama2::services::alert::core::Notifier> NotifierPtr;

        class DataManager;
        //! Shared smart pointer for DataManager
        typedef std::shared_ptr<terrama2::services::alert::core::DataManager> DataManagerPtr;
      } /* core */
    }   /* alert */
  }
}

#endif // __TERRAMA2_SERVICES_ALERT_SHARED_HPP__
