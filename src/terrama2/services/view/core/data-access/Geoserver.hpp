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
  \file terrama2/services/view/core/data-access/Geoserver.hpp

  \brief Communication class between View service and GeoServer

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_VIEW_CORE_GEOSERVER_HPP__
#define __TERRAMA2_SERVICES_VIEW_CORE_GEOSERVER_HPP__

// TerraMA2
#include "../View.hpp"
#include "../MapsServer.hpp"
#include "../Shared.hpp"
#include "../Typedef.hpp"
#include "../ViewLogger.hpp"
#include "../../../../core/Typedef.hpp"

// TerraLib
#include <terralib/core/uri/URI.h>
#include <terralib/se/Style.h>
#include <terralib/geometry/Envelope.h>
#include <terralib/datatype/DateTimeProperty.h>

// STL
#include <string>
#include <map>

namespace terrama2
{
  namespace core
  {
    class ProcessLogger;
  }
  namespace services
  {
    namespace view
    {
      namespace core
      {
        /*!
         * \brief Defines a struct to store helpers while perform GeoServer configuration
         */
        struct RasterInfo
        {
          RasterInfo() = default;
          RasterInfo(RasterInfo&&) = default;
          RasterInfo(const RasterInfo&) = delete;
          RasterInfo& operator=(const RasterInfo&) = delete;

          std::string name; //!< Defines Raster Name
          int srid; //!< Raster SRID
          double resolutionX; //!< Represents Raster Resolution X Pixel
          double resolutionY; //!< Represents Raster Resolution Y Pixel
          te::dt::TimeInstant timeTz; //!< Raster Time Instant TZ
          std::unique_ptr<te::gm::Envelope> envelope; //!< Raster envelope limits
        };

        /*!
            \brief GeoServer class for working with GeoServer through RESTful.

            This class stores a address to a GeoServer and the workspace in the server.
          */
        class GeoServer : public MapsServer
        {
          public:

            /*!
             * \brief GeoServer class constructor
             * \param uri The address of the GeoServer. Format: http://user:password@localhost:0000/geoserver
             * \param workspace A workspace to work in GeoServer
             */
            GeoServer(const te::core::URI uri);

            /*!
             * \brief GeoServer class default destructor
             */
            virtual ~GeoServer() = default;

            static MapsServerPtr make(te::core::URI uri);

            static MapsServerType mapsServerType(){ return "GEOSERVER"; }

        protected:
            /*!
             * \brief Return the working uri
             * \return Return the working uri
             */
            const te::core::URI& uri() const;

            /*!
             * \brief Set a workspace to work in GeoServer
             * \param workspace The workspace name
             */
            void setWorkspace(const std::string& workspace);


            const std::string& getWorkspace(const std::string& name) const;

            /*!
             * \brief Register the wanted workspace in GeoServer.
             *        It will also set the new name to the class workspace
             * \param name The workspace name
             */
            void registerWorkspace(const std::string& name = std::string());

            /*!
             * \brief Return the working workspace
             * \return Return the working workspace
             */
            const std::string& workspace() const;

            /*!
             * \brief This method register a PostGIS data Store and table as a layer in GeoServer
             * \param dataStoreName The name of the store  to create in GeoServer
             * \param connInfo The connection parameters to the Postgis BD
             * \param tableName The name of the table in Postgis to register in GeoServer
             * \param title
             * \param timestampPropertyName The datetime property for temporal data
             * \param sql The SQL statements to create the view
             */
            void registerPostgisTable(const ViewPtr viewPtr,
                                      const std::string& dataStoreName,
                                      terrama2::core::DataSeriesType dataSeriesType,
                                      std::map<std::string, std::string> connInfo,
                                      const std::string& tableName,
                                      const std::string& layerName,
                                      const std::unique_ptr<te::da::DataSetType>& dataSetType,
                                      const std::string& timestampPropertyName = "",
                                      const std::string& sql = "") const;

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
            void registerVectorFile(const ViewPtr& viewPtr,
                                    const std::string& dataStoreName,
                                    const std::string& shpFilePath,
                                    const std::string& layerName) const;

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
                                      const std::string& coverageName,
                                      const std::string& extension) const;

            void registerMosaicCoverage(const ViewPtr viewPtr,
                                        const std::string& coverageStoreName,
                                        const std::string& mosaicPath,
                                        const std::string& coverageName,
                                        const RasterInfo& rasterInfo,
                                        const std::string& style = "",
                                        const std::string& configure = "all") const;

            std::unique_ptr<te::se::Style> generateVectorialStyle(const View::Legend& legend,
                                                                  const te::gm::GeomType& geomType) const;


            /*!
             * \brief Removes GeoServer Workspace.
             *
             * Since TerraMA² View Service generates a individual workspace for each view, you can use this method
             * to remove entire workspace in GeoServer environment.
             *
             * \param v - Current view id object to remove. Default is selected workspace
             */
            void cleanup(const ViewPtr& viewPtr = nullptr,
                         terrama2::core::DataProviderPtr dataProvider = nullptr,
                         std::shared_ptr<terrama2::core::ProcessLogger> logger = nullptr) override;


            /*!
             * \brief Method to register a style in the GeoServer from a text file
             * \param name The name of the style
             * \param styleFilePath The full path to the txt file
             */
            void registerStyleFile(const std::string& name, const std::string& styleFilePath) const;

            /*!
             * \brief Method to register a style in the GeoServer from a TerraLib Style object
             * \param name The name of the style
             * \param style The Terralib Style object
             */
            void registerStyle(const std::string& name, const std::unique_ptr<te::se::Style> &style) const;

            /*!
             * \brief Method to register a style in the GeoServer from a TerraLib Style object
             * \param name The name of the style
             * \param style The style XML
             */
            void registerStyle(const std::string& name, const std::string &style, const std::string& sldVersion = "1.1.0") const;


            void registerStyle(const std::string& name,
                               const View::Legend& legend,
                               const View::Legend::ObjectType&objectType,
                               const te::gm::GeomType& geomType = te::gm::UnknownGeometryType) const;

            /*!
             * \brief Method to delete a vector file in Geoserver
             * \param dataStoreName The store name that is located the file
             * \param fileName The name of the vector file to delete
             * \param recursive If true will delete all layers associated with this vetctor file in server
             */
            void deleteVectorLayer(const std::string& dataStoreName, const std::string& fileName, bool recursive) const;

            /*!
             * \brief Method to delete a coverage file in Geoserver
             * \param coverageStoreName The store name that is located the file
             * \param fileName The name of the coverage file to delete
             * \param recursive If true will delete all layers associated with this coverage file in server
             */
            void deleteCoverageLayer(const std::string& coverageStoreName, const std::string& fileName, bool recursive) const;

            /*!
             * \brief Method to delete a style file in Geoserver
             * \param styleName The name of style to delete
             */
            void deleteStyle(const std::string& styleName) const;

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
                           const std::list<std::pair<std::string, std::string> > layersAndStyles,
                           const te::gm::Envelope env,
                           const uint32_t width,
                           const uint32_t height,
                           const uint32_t srid,
                           const std::string& format) const;

            /*!
             * \brief getDataStore
             * \param name The name of the store in GeoServer
             * \return
             */
            const std::string& getDataStore(const std::string& name) const;

            /*!
             * \brief getCoverageStore
             * \param name The name of the store in GeoServer
             * \return
             */
            const std::string& getCoverageStore(const std::string& name) const;

            /*!
             * \brief getFeature
             * \param dataStoreName The name of the store in GeoServer
             * \param name The name of the layer in GeoServer
             * \return
             */
            const std::string& getFeature(const std::string& dataStoreName,const std::string& name) const;

            /*!
             * \brief This method register a PostGIS data Store in GeoServer
             * \param dataStoreName The name of the store in GeoServer,
             *                      if it doesn't exists, will be created.
             * \param connInfo The connection parameters to the Postgis BD
             */
            void registerPostGisDataStore(const std::string& dataStoreName,
                                          const std::map<std::string, std::string> connInfo) const;

            void registerVectorDataStore(const std::string& dataStoreName,
                                         const std::string& shpFilePath) const;

            virtual QJsonObject generateLayersInternal(const ViewPtr viewPtr,
                                                       const std::pair<terrama2::core::DataSeriesPtr, terrama2::core::DataProviderPtr>& dataSeriesProvider,
                                                       const std::shared_ptr<DataManager> dataManager,
                                                       std::shared_ptr<ViewLogger> logger,
                                                       const RegisterId logId) override;

            void registerLayerDefaultStyle(const std::string& styleName,
                                           const std::string& layerName) const;

            std::string getGeomTypeString(const te::gm::GeomType& geomType) const;

            std::vector<std::string> registerMosaics(const terrama2::core::DataProviderPtr inputDataProvider,
                                                     const terrama2::core::DataSeriesPtr inputDataSeries,
                                                     const std::shared_ptr<DataManager> dataManager,
                                                     const ViewPtr viewPtr, const te::core::URI& connInfo) const;

            void createGeoserverPropertiesFile(const std::string& outputFolder,
                                               const std::string& exhibitionName,
                                               DataSeriesId dataSeriesId) const;

            /*!
             * \brief createGeoserverTempMosaic
             * \param dataManager
             * \param dataset
             * \param filter
             * \param exhibitionName
             * \param outputFolder
             * \return Return the geometry SRID
             */
            int createGeoserverTempMosaic(terrama2::core::DataManagerPtr dataManager,
                                          terrama2::core::DataSetPtr dataset,
                                          const terrama2::core::Filter& filter,
                                          const std::string& exhibitionName,
                                          const std::string& outputFolder) const;

            std::string createPostgisDatastorePropertiesFile(const std::string& outputFolder,
                                                      const te::core::URI& connInfo) const;

            std::string createPostgisMosaicLayerPropertiesFile(const std::string& outputFolder,
                                                               const std::string& exhibitionName,
                                                               const RasterInfo& rasterInfo) const;

            void createPostgisIndexerPropertiesFile(const std::string& outputFolder,
                                                    const std::string& exhibitionName) const;

            void createTimeregexPropertiesFile(const std::string& outputFolder,
                                               const std::string& regex) const;

            /*!
             * \brief
             * \param dataManager
             * \param dataset
             * \param filter
             * \return
             */
            std::vector<RasterInfo> getRasterInfo(terrama2::core::DataManagerPtr dataManager,
                                                  terrama2::core::DataSetPtr dataset,
                                                  const terrama2::core::Filter& filter) const;

            void createMosaicTable(std::shared_ptr<te::da::DataSource> transactor,
                                   const std::string& tableName,
                                   int srid) const;

          private:
            /*!
             * \brief Retrieves a unique workspace name using view id.
             *
             * \param id - View identifier
             * \return Workspace name: "terrama2_ViewId"
             */
            std::string workspaceName(const ViewPtr& viewPtr);

            /*!
             * \brief Helper to retrieve common view name with view id.
             * \param id View identifier
             * \return Unique Layer Name
             */
            std::string viewLayerName(const ViewPtr& viewPtr) const;

            std::string getAttributeName(const View::Legend& legend) const;

            std::string viewLayerTitle(const ViewPtr& viewPtr) const;


            std::string workspace_ = "terrama2"; /*!< A workspace to work in GeoServer */
        };
      }
    }
  }
}

#endif //__TERRAMA2_SERVICES_VIEW_CORE_GEOSERVER_HPP__
