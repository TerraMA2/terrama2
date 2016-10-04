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
      namespace da
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
           * \param uri The address of the GeoServer. Format: http://user:password@localhost:0000/geoserver
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
           * \brief Register the wanted workspace in GeoServer.
           *        It will also set the new name to the class workspace
           * \param name The workspace name
           */
          void registerWorkspace(const std::string& name);

          /*!
           * \brief This method register a table in a PostGis BD as a layer in GeoServer
           * \param dataStoreName The name of the store in GeoServer,
           *                      if it doesn't exists, will be created.
           * \param connInfo The connection parameters to the Postgis BD
           * \param tableName The name of the table in Postgis to register in GeoServer
           */
          void registerPostgisTable(const std::string& dataStoreName,
                                    std::map<std::string, std::string> connInfo,
                                    const std::string& tableName) const;

          /*!
           * \brief Method to upload a .zip with the vector files from out of the server to the GeoServer
           * \param dataStoreName The name of the store in GeoServer to upload the vector files,
           *                      if it doesn't exists, will be created.
           * \param shpZipFilePath The full path to the .zip with the vector files
           * \param extension  Specifies the type of data being uploaded. Supported: shp, properties, h2, spatialite, appschema
           */
          void uploadZipVectorFiles(const std::string& dataStoreName,
                                    const std::string& shpZipFilePath,
                                    const std::string& extension) const;

          /*!
           * \brief Register an vector file that is located at the server
           * \param dataStoreName The name of the store in GeoServer to register the vector file,
           *                      if it doesn't exists, will be created.
           * \param shpFilePath The full path to the vector file
           * \param extension Specifies the type of data being registered. Supported: shp, properties, h2, spatialite, appschema
           */
          void registerVectorFile(const std::string& dataStoreName,
                                  const std::string& shpFilePath,
                                  const std::string& extension) const;

          /*!
           * \brief Register a folder that contains vector files
           * \param dataStoreName The name of the store in GeoServer to register the vectors files inside the folder,
           *                      if it doesn't exists, will be created.
           * \param shpFolderPath The full path to the folder with the vector files
           * \param extension Specifies the type of data being registered. Supported: shp, properties, h2, spatialite, appschema
           */
          void registerVectorsFolder(const std::string& dataStoreName,
                                     const std::string& shpFolderPath,
                                     const std::string& extension) const;

          /*!
           * \brief Method to upload a .zip with a coverage file from out of the server to the GeoServer.
           *        One coverage per .zip.
           * \param coverageStoreName The name of the store in GeoServer to upload the coverage,
           *                            if it doesn't exists, will be created.
           * \param coverageFilePath The full path to the .zip with the coverage file
           * \param extension  Specifies the type of data being uploaded. Supported: geotiff, worldimage(JPEG, PNG, TIFF), imagemosaic
           */
          void uploadZipCoverageFile(const std::string& coverageStoreName,
                                     const std::string& coverageZipFilePath,
                                     const std::string& extension) const;

          /*!
           * \brief Register an coverage file that is located at the server
           * \param coverageStoreName The name of the store in GeoServer to register the coverage file,
           *                            if it doesn't exists, will be created.
           * \param coverageFilePath The full path to the coverage file
           * \param extension Specifies the type of data being uploaded. Supported: geotiff, worldimage(JPEG, PNG, TIFF), imagemosaic
           */
          void registerCoverageFile(const std::string& coverageStoreName,
                                    const std::string& coverageFilePath,
                                    const std::string& extension) const;

          /*!
           * \brief Method to register a style in the GeoServer from a text file
           * \param name The name of the style
           * \param styleFilePath The full path to the txt file
           */
          void registerStyle(const std::string& name, const std::string& styleFilePath) const;

          /*!
           * \brief Method to register a style in the GeoServer from a TerraLib Style object
           * \param name The name of the style
           * \param style The Terralib Style object
           */
          void registerStyle(const std::string& name, const std::unique_ptr<te::se::Style> &style) const;

          /*!
           * \brief Method to delete a workspace in Geoserver
           * \param recursive If true will delete all data associated with this workspace in server
           */
          void deleteWorkspace(bool recursive) const;

          /*!
           * \brief Method to delete a vector file in Geoserver
           * \param dataStoreName The store name that is located the file
           * \param fileName The name of the vector file to delete
           * \param recursive If true will delete all layers associated with this vetctor file in server
           */
          void deleteVectorFile(const std::string& dataStoreName, const std::string& fileName, bool recursive) const;

          /*!
           * \brief Method to delete a coverage file in Geoserver
           * \param coverageStoreName The store name that is located the file
           * \param fileName The name of the coverage file to delete
           * \param recursive If true will delete all layers associated with this coverage file in server
           */
          void deleteCoverageFile(const std::string& coverageStoreName, const std::string& fileName, bool recursive) const;

          /*!
           * \brief Method to delete a style file in Geoserver
           * \param styleName The name of style to delete
           */
          void deleteStyle(const::std::string& styleName) const;


          /*!
           * \brief Method to download a image with the layers registered in Geoserver
           * \param savePath Path to save the image
           * \param fileName Image name to save
           * \param layersAndStyles The layers and theirs styles
           * \param env The bounding box of image
           * \param width The image width resolution
           * \param height The height resolution of the image
           * \param srid The SRID
           * \param format Format to save the image.
           */
          void getMapWMS(const std::string& savePath,
                         const std::string& fileName,
                         const std::list<std::pair<std::string, std::string>> layersAndStyles,
                         const te::gm::Envelope env,
                         const uint32_t width,
                         const uint32_t height,
                         const uint32_t srid,
                         const std::string& format) const;

        private:

          te::core::URI uri_;     /*!< The address of the GeoServer */
          std::string workspace_; /*!< A workspace to work in GeoServer */

        };
      }
    }
  }
}

#endif //__TERRAMA2_SERVICES_VIEW_CORE_GEOSERVER_HPP__
