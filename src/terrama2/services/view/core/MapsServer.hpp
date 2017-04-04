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
  \file terrama2/services/view/core/MapsServer.hpp

  \brief

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_VIEW_CORE_MAPS_SERVER_HPP__
#define __TERRAMA2_SERVICES_VIEW_CORE_MAPS_SERVER_HPP__

// TerraMA2
#include "Shared.hpp"
#include "ViewLogger.hpp"
#include "../../../core/Shared.hpp"
#include "../../../core/Typedef.hpp"

// TerraLib
#include <terralib/core/uri/URI.h>

// Qt
#include <QJsonArray>

// STD
#include <unordered_map>

namespace terrama2
{
  namespace services
  {
    namespace view
    {
      namespace core
      {
        class MapsServer
        {
          public:

            MapsServer(te::core::URI uri) : uri_(uri) {}

            virtual ~MapsServer() = default;

            virtual QJsonObject generateLayers(const ViewPtr viewPtr,
                                               const std::pair< terrama2::core::DataSeriesPtr, terrama2::core::DataProviderPtr >& dataSeriesProviders,
                                               const std::shared_ptr<DataManager> dataManager,
                                               std::shared_ptr<ViewLogger> logger,
                                               const RegisterId logId) = 0;

          protected:

            te::core::URI uri_;     /*!< The address and authentication info of the Maps Server */
        };
      }
    }
  }

}


#endif //__TERRAMA2_SERVICES_VIEW_CORE_MAPS_SERVER_HPP__
