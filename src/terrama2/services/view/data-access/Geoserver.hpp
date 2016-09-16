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
  \file terrama2/services/view/data-access/Geoserver.hpp

  \brief Communication class between View service and GeoServer

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_VIEW_CORE_GEOSERVER_HPP__
#define __TERRAMA2_SERVICES_VIEW_CORE_GEOSERVER_HPP__

// STL
#include <string>

// TerraLib
#include <terralib/core/uri/URI.h>
#include <terralib/se/Style.h>

namespace terrama2
{
  namespace services
  {
    namespace view
    {
      namespace data_access
      {
        class GeoServer
        {

        public:

          GeoServer(const te::core::URI uri, const std::string workspace = std::string());

          ~GeoServer() = default;

          void registerWorkspace(const std::string& name);

          void uploadVectorFile(const std::string& name, const std::string& shpPath);

          void registerStyle(const std::string& name, const std::string& styleFilePath);

          void registerStyle(const std::string& name, const std::unique_ptr<te::se::Style> &style);

        private:

          te::core::URI uri_;
          std::string workspace_;

        };
      }
    }
  }
}



#endif //__TERRAMA2_SERVICES_VIEW_CORE_GEOSERVER_HPP__
