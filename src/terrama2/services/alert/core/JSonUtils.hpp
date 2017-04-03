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
  \file src/terrama2/services/alert/core/JSonUtils.hpp

  \brief

  \author Jano Simas
*/

#ifndef __TERRAMA2_SERVICES_ALERT_CORE_JSONUTILS_HPP__
#define __TERRAMA2_SERVICES_ALERT_CORE_JSONUTILS_HPP__

#include "Alert.hpp"
#include "Shared.hpp"
#include "../../../core/data-model/DataManager.hpp"

// Qt
#include <QJsonObject>

namespace terrama2
{
  namespace services
  {
    namespace alert
    {
      namespace core
      {
        /*!
          \brief Creates a Alert from a QJsonObject.
          \see Alert for json structure
          \see [Alert at Trac](https://trac.dpi.inpe.br/terrama2/wiki/programmersguide/architecture/services/alert/Alert) for more information.
          */
        AlertPtr fromAlertJson(QJsonObject json, terrama2::core::DataManager* dataManager);

        /*!
          \brief Creates a Json object from a Alert
          \attention This is a function created for debug and tests
        */
        QJsonObject toJson(AlertPtr collector);

      } // end namespace core
    }   // end namespace collector
  }     // end namespace services
}       // end namespace terrama2

#endif  // __TERRAMA2_SERVICES_ALERT_CORE_JSONUTILS_HPP__
