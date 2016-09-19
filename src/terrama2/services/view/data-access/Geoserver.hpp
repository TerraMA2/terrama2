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
        /*!
          \brief GeoServer class for working with GeoServer through RESTful.

          This class stores a address to a GeoServer and the workspace in the server.
        */
        class GeoServer
        {
        public:

          /*!
           * \brief GeoServer class constructor
           * \param uri The address of the GeoServer
           * \param workspace A workspace to work in GeoServer
           */
          GeoServer(const te::core::URI uri, const std::string workspace = "terrama");

          /*!
           * \brief GeoServer class default destructor
           */
          ~GeoServer() = default;

          /*!
           * \brief Set a workspace to work in GeoServer
           * \param workspace The workspace name
           */
          void setWorkspace(const std::string& workspace);

          /*!
           * \brief Register the wanted workspace in GeoServer
           * \param name The workspace name
           */
          void registerWorkspace(const std::string& name);

          /*!
           * \brief Method to upload a .zip with the vector files to the GeoServer
           * \param dataStoreName The name of the store in GeoServer to upload the vector files,
           *                      if it doesn't exists, will be created.
           * \param shpPath The full path to the .zip with the vector files
           * \param extension  Specifies the type of data being uploaded. Supported: shp, properties, h2, spatialite, appschema
           */
          void uploadVectorFile(const std::string& dataStoreName, const std::string& shpPath, const std::string& extension);

          /*!
           * \brief Method to upload a .zip with the coverage files to the GeoServer
           * \param coverageStoreName The name of the store in GeoServer to upload the coverage files,
           *                            if it doesn't exists, will be created.
           * \param shpPath The full path to the .zip with the coverage files
           * \param extension  Specifies the type of data being uploaded. Supported: geotiff, worldimage(JPEG, PNG, TIFF), imagemosaic
           */
          void uploadCoverageFile(const std::string& coverageStoreName, const std::string& styleFilePath, const std::string& extension);

          /*!
           * \brief Method to register a style in the GeoServer from a text file
           * \param name The name of the style
           * \param styleFilePath The full path to the txt file
           */
          void registerStyle(const std::string& name, const std::string& styleFilePath);

          /*!
           * \brief Method to register a style in the GeoServer from a TerraLib Style object
           * \param name The name of the style
           * \param style The Terralib Style object
           */
          void registerStyle(const std::string& name, const std::unique_ptr<te::se::Style> &style);

        private:

          te::core::URI uri_;     /*!< The address of the GeoServer */
          std::string workspace_; /*!< A workspace to work in GeoServer */

        };
      }
    }
  }
}

#endif //__TERRAMA2_SERVICES_VIEW_CORE_GEOSERVER_HPP__
