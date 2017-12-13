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
  \file terrama2/services/view/core/data-access/Geoserver.cpp

  \brief Communication class between View service and GeoServer

  \author Vinicius Campanha
*/

// TerraMA2
#include "Geoserver.hpp"
#include "Exception.hpp"
#include "DataAccess.hpp"
#include "../Utils.hpp"
#include "../DataManager.hpp"
#include "../serialization/Serialization.hpp"
#include "../../../../impl/DataAccessorFile.hpp"
#include "../../../../core/data-model/DataProvider.hpp"
#include "../../../../core/utility/Logger.hpp"
#include "../../../../core/utility/DataAccessorFactory.hpp"
#include "../../../../core/utility/Utils.hpp"
#include "../../../../core/utility/Raii.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/ws/core/CurlWrapper.h>
#include <terralib/ws/ogc/wms/client/WMSClient.h>
#include <terralib/geometry/Envelope.h>
#include <terralib/geometry/Utils.h>
#include <terralib/memory/DataSetItem.h>
#include <terralib/datatype/StringProperty.h>
#include <terralib/geometry/GeometryProperty.h>

// TerraLib FE - Style Filter Operator
#include <terralib/fe/PropertyName.h>
#include <terralib/fe/Literal.h>
#include <terralib/fe/BinaryComparisonOp.h>
#include <terralib/fe/Filter.h>
#include <terralib/fe/And.h>
#include <terralib/fe/Globals.h>

#include <terralib/se/Rule.h>
#include <terralib/se/FeatureTypeStyle.h>

#include <terralib/raster/RasterProperty.h>
#include <terralib/raster/Grid.h>

// Qt
#include <QTemporaryFile>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QString>

terrama2::services::view::core::GeoServer::GeoServer(const te::core::URI uri)
  : MapsServer(uri)
{

}


const te::core::URI& terrama2::services::view::core::GeoServer::uri() const
{
  return uri_;
}


QJsonObject terrama2::services::view::core::GeoServer::generateLayersInternal(const ViewPtr viewPtr,
                                                                      const std::pair< terrama2::core::DataSeriesPtr, terrama2::core::DataProviderPtr >& dataSeriesProvider,
                                                                      const std::shared_ptr<DataManager> dataManager,
                                                                      std::shared_ptr<ViewLogger> logger,
                                                                      const RegisterId logId)
{
  QJsonObject jsonAnswer;

  terrama2::core::DataSeriesPtr inputDataSeries = dataSeriesProvider.first;
  terrama2::core::DataProviderPtr inputDataProvider = dataSeriesProvider.second;

  // Check if the view belongs to this dataSeries
  if(inputDataSeries->id != viewPtr->dataSeriesID)
  {
    QString errorMsg = QString("This View is not from this Data Series.");
    logger->log(ViewLogger::ERROR_MESSAGE, errorMsg.toStdString(), logId);
    TERRAMA2_LOG_ERROR() << QObject::tr(errorMsg.toStdString().c_str());
    throw ViewGeoserverException() << ErrorDescription(errorMsg);
  }

  // Check if the view can be done by the maps server
  DataProviderType dataProviderType = inputDataProvider->dataProviderType;
  if(dataProviderType != "POSTGIS" && dataProviderType != "FILE")
  {
    QString errorMsg = QString("Data provider not supported: %1.").arg(dataProviderType.c_str());
    logger->log(ViewLogger::ERROR_MESSAGE, errorMsg.toStdString(), logId);
    TERRAMA2_LOG_ERROR() << QObject::tr(errorMsg.toStdString().c_str());
    throw ViewGeoserverException() << ErrorDescription(errorMsg);
  }

  DataFormat dataFormat = inputDataSeries->semantics.dataFormat;
  if(dataFormat != "OGR" && dataFormat != "POSTGIS" && dataFormat != "GDAL")
  {
    QString errorMsg = QString("Data format not supported in the maps server: %1.").arg(dataFormat.c_str());
    logger->log(ViewLogger::ERROR_MESSAGE, errorMsg.toStdString(), logId);
    TERRAMA2_LOG_ERROR() << QObject::tr(errorMsg.toStdString().c_str());
    throw ViewGeoserverException() << ErrorDescription(errorMsg);
  }

  if(inputDataSeries->datasetList.empty())
  {
    logger->log(ViewLogger::WARNING_MESSAGE, "No data to register.", logId);
    TERRAMA2_LOG_WARNING() << QObject::tr("No data to register in maps server.");
    return jsonAnswer;
  }

  // If it has a legend, performs extra validations
  if (viewPtr->legend != nullptr)
  {
    auto& legend = viewPtr->legend;

    auto it = legend->metadata.find("creation_type");
    if (it == legend->metadata.end())
      throw ViewGeoserverException() << ErrorDescription("The band number is required.");

    // Predefined styles as XML styles
    View::Legend::CREATION_TYPE creationType = it->second == "editor" ? View::Legend::CREATION_TYPE::EDITOR
                                                                      : View::Legend::CREATION_TYPE::XML;

    // For Grid Legends, a Band value is required and then it must be valid. (band_number >= 0)
    if (inputDataSeries->semantics.dataSeriesType == terrama2::core::DataSeriesType::GRID &&
        creationType == View::Legend::CREATION_TYPE::EDITOR)
    {
      auto it = legend->metadata.find("band_number");
      if (it == legend->metadata.end())
        throw ViewGeoserverException() << ErrorDescription(QObject::tr("No band number provided"));

      try
      {
        int band_number = std::stoi(it->second);
        if (band_number < 0)
          throw ViewGeoserverException();
      }
      catch(const std::exception&)
      {
        throw ViewGeoserverException() <<
              ErrorDescription(QObject::tr("The band number provided %1 is invalid.")
                                          .arg(QString::fromStdString(it->second)));
      }
    }
  }

  setWorkspace(workspaceName(viewPtr));

  registerWorkspace();

  QJsonArray layersArray;

  // Variables to hel in style creation
  View::Legend::ObjectType objectType = View::Legend::ObjectType::UNKNOWN;
  te::gm::GeomType geomType = te::gm::UnknownGeometryType;

  if(dataProviderType == "FILE")
  {
    const auto& temporality = inputDataSeries->semantics.temporality;

    if(temporality == terrama2::core::DataSeriesTemporality::DYNAMIC && dataFormat == "GDAL")
    {
      const auto& layersNames = registerMosaics(inputDataProvider, inputDataSeries, dataManager, viewPtr, logger->getConnectionInfo());

      for(const auto& layerName : layersNames)
      {
        QJsonObject layer;
        layer.insert("layer", QString::fromStdString(layerName));
        layersArray.push_back(layer);
      }

      objectType = View::Legend::ObjectType::RASTER;
    }
    else
    {
      for(auto& dataset : inputDataSeries->datasetList)
      {
        // Get the list of layers to register
        auto fileInfoList = DataAccess::getFilesList(dataSeriesProvider, dataset, viewPtr->filter);

        if(fileInfoList.empty())
        {
          QString errorMsg = QString("No data in data series %1.").arg(inputDataSeries->id);
          logger->log(ViewLogger::WARNING_MESSAGE, errorMsg.toStdString(), logId);
          TERRAMA2_LOG_WARNING() << QObject::tr(errorMsg.toStdString().c_str());
          continue;
        }

        for(auto& fileInfo : fileInfoList)
        {
          std::string layerName = viewLayerName(viewPtr);

          if(dataFormat == "OGR")
          {
            if(objectType ==  View::Legend::ObjectType::UNKNOWN)
            {
              std::unique_ptr< te::da::DataSetType > dataSetType(DataAccess::getVectorialDataSetType(fileInfo));
              if(!dataSetType)
              {
                QString errorMsg = QString("Unable to access data series %1.").arg(inputDataSeries->id);
                logger->log(ViewLogger::ERROR_MESSAGE, errorMsg.toStdString(), logId);
                TERRAMA2_LOG_ERROR() << QObject::tr(errorMsg.toStdString().c_str());
                break;
              }

              auto geomProperty = te::da::GetFirstGeomProperty(dataSetType.get());

              if(geomProperty != nullptr)
                geomType = geomProperty->getGeometryType();

              objectType = View::Legend::ObjectType::GEOMETRY;
            }

            registerVectorFile(viewPtr,
                               layerName + "_datastore_" + std::to_string(viewPtr->id),
                               fileInfo.absoluteFilePath().toStdString(),
                               layerName);
          }
          else if(dataFormat == "GDAL")
          {
            if(objectType ==  View::Legend::ObjectType::UNKNOWN)
            {
              std::unique_ptr< te::da::DataSetType > dataSetType(DataAccess::getGeotiffDataSetType(fileInfo));

              if(dataSetType->hasRaster())
                objectType = View::Legend::ObjectType::RASTER;
            }

            registerCoverageFile(layerName + "_coveragestore_" + std::to_string(viewPtr->id),
                                 fileInfo.absoluteFilePath().toStdString(),
                                 layerName,
                                 "geotiff");
          }

          QJsonObject layer;
          layer.insert("layer", QString::fromStdString(layerName));
          layersArray.push_back(layer);
        }
      }
    }
  }
  else if(dataProviderType == "POSTGIS")
  {
    QUrl url(inputDataProvider->uri.c_str());
    std::map<std::string, std::string> connInfo
    {
      {"PG_HOST", url.host().toStdString()},
      {"PG_PORT", std::to_string(url.port())},
      {"PG_USER", url.userName().toStdString()},
      {"PG_PASSWORD", url.password().toStdString()},
      {"PG_DB_NAME", url.path().section("/", 1, 1).toStdString()},
      {"PG_CONNECT_TIMEOUT", "4"},
      {"PG_CLIENT_ENCODING", "UTF-8"}
    };

    if(inputDataSeries->semantics.dataSeriesType == terrama2::core::DataSeriesType::DCP)
    {
      objectType = View::Legend::ObjectType::GEOMETRY;
      geomType = te::gm::PointType;
      terrama2::core::DataProviderPtr inputObjectProvider = dataManager->findDataProvider(inputDataSeries->dataProviderId);

      auto dcpPositions = DataAccess::getDCPPostgisTableInfo(inputDataSeries, inputObjectProvider);
      std::string variable = getAttributeName(*viewPtr->legend);
      std::string SQL = "SELECT t.id, t.geom, t.timestamp, t.var as "+variable+" from dcp_last_measures('"+dcpPositions.tableName+"', '"+variable+"')"
            "AS t(id integer, geom geometry, \"timestamp\" timestamp with time zone, var double precision)";

      std::unique_ptr<te::da::DataSetType> modelDataSetType(dcpPositions.dataSetType.release());

      TableInfo tableInfo = DataAccess::getDCPPostgisTableInfo(inputDataSeries, inputObjectProvider);
      std::string layerName = viewLayerName(viewPtr);

      registerPostgisTable(viewPtr,
                           std::to_string(viewPtr->id) + "_" + std::to_string(inputDataSeries->id) + "_datastore",
                           inputDataSeries->semantics.dataSeriesType,
                           connInfo,
                           tableInfo.tableName,
                           layerName,
                           modelDataSetType,
                           "",
                           SQL);

      QJsonObject layer;
      layer.insert("layer", QString::fromStdString(layerName));
      layersArray.push_back(layer);
    }
    else
    {
    for(auto& dataset : inputDataSeries->datasetList)
    {
      TableInfo tableInfo = DataAccess::getPostgisTableInfo(dataset, inputDataSeries, inputDataProvider);

      std::string tableName = tableInfo.tableName;
      std::string layerName = viewLayerName(viewPtr);
      std::string timestampPropertyName = tableInfo.timestampPropertyName;

      std::unique_ptr<te::da::DataSetType> modelDataSetType = std::move(tableInfo.dataSetType);

      std::string SQL = "";

      if(inputDataSeries->semantics.dataSeriesType == terrama2::core::DataSeriesType::ANALYSIS_MONITORED_OBJECT)
      {
        const auto& id = dataset->format.find("monitored_object_id");
        if(id == dataset->format.end())
        {
          logger->log(ViewLogger::ERROR_MESSAGE, "Data to join not informed.", logId);
          TERRAMA2_LOG_ERROR() << QObject::tr("Cannot join data from a different DB source!");
          continue;
        }

        terrama2::core::DataSeriesPtr monitoredObjectDataSeries = dataManager->findDataSeries(std::stoi(id->second));
        terrama2::core::DataProviderPtr monitoredObjectProvider = dataManager->findDataProvider(monitoredObjectDataSeries->dataProviderId);

        QUrl monitoredObjectUrl(monitoredObjectProvider->uri.c_str());

        if(monitoredObjectUrl.host() != url.host()
           || monitoredObjectUrl.port() != url.port()
           || monitoredObjectUrl.path().section("/", 1, 1) != url.path().section("/", 1, 1))
        {
          QString errMsg = QObject::tr("Cannot join data from a different DB source.");
          logger->log(ViewLogger::ERROR_MESSAGE, errMsg.toStdString(), logId);
          TERRAMA2_LOG_ERROR() << errMsg;
          continue;
        }

        if(monitoredObjectDataSeries->datasetList.empty())
        {
          QString errMsg = QObject::tr("No dataset found in dataseries: %1.").arg(QString::fromStdString(monitoredObjectDataSeries->name));
          logger->log(ViewLogger::ERROR_MESSAGE, errMsg.toStdString(), logId);
          TERRAMA2_LOG_ERROR() << errMsg;
          continue;
        }

        TableInfo monitoredObjectTableInfo;
        if(monitoredObjectDataSeries->semantics.dataSeriesType == terrama2::core::DataSeriesType::GEOMETRIC_OBJECT)
        {
          const terrama2::core::DataSetPtr monitoredObjectDataset = monitoredObjectDataSeries->datasetList.at(0);

          monitoredObjectTableInfo = DataAccess::getPostgisTableInfo(monitoredObjectDataset, monitoredObjectDataSeries, monitoredObjectProvider);
        }
        else if (monitoredObjectDataSeries->semantics.dataSeriesType == terrama2::core::DataSeriesType::DCP)
        {
          monitoredObjectTableInfo = DataAccess::getDCPPostgisTableInfo(monitoredObjectDataSeries, monitoredObjectProvider);
        }
        else
        {
          QString errMsg = QObject::tr("Invalid type of dataseries: %1.").arg(QString::fromStdString(monitoredObjectDataSeries->name));
          logger->log(ViewLogger::ERROR_MESSAGE, errMsg.toStdString(), logId);
          TERRAMA2_LOG_ERROR() << errMsg;
          continue;
        }

        std::string pk = monitoredObjectTableInfo.dataSetType->getPrimaryKey()->getProperties().at(0)->getName();

        auto& propertiesVector = monitoredObjectTableInfo.dataSetType->getProperties();

        SQL = "SELECT ";

        for(auto& property : propertiesVector)
        {
          const std::string& propertyName = property->getName();
          SQL += "t1." + propertyName + " as monitored_" + propertyName + ", ";
        }

        SQL += "t2.* ";
        SQL += "FROM " + monitoredObjectTableInfo.tableName;
        SQL += " as t1 , " + tableName + " as t2 ";
        SQL += "WHERE t1." + pk + " = t2." + pk;

        modelDataSetType.reset(monitoredObjectTableInfo.dataSetType.release());
        tableName = layerName;
      }

        registerPostgisTable(viewPtr,
                             std::to_string(viewPtr->id) + "_" + std::to_string(inputDataSeries->id) + "_datastore",
                           inputDataSeries->semantics.dataSeriesType,
                           connInfo,
                           tableName,
                           layerName,
                           modelDataSetType,
                           timestampPropertyName,
                           SQL);

      if(objectType ==  View::Legend::ObjectType::UNKNOWN)
      {
        if(modelDataSetType->hasGeom())
        {
          auto geomProperty = te::da::GetFirstGeomProperty(modelDataSetType.get());

          if(geomProperty != nullptr)
            geomType = geomProperty->getGeometryType();

          objectType = View::Legend::ObjectType::GEOMETRY;
        }
        else if(modelDataSetType->hasRaster())
        {
          objectType = View::Legend::ObjectType::RASTER;
        }
      }

      QJsonObject layer;
      layer.insert("layer", QString::fromStdString(layerName));
      layersArray.push_back(layer);
      }
    }
  }

  if(viewPtr->legend)
  {
    // Register style
    std::string styleName = "";

    std::string layerName = viewLayerName(viewPtr);
    styleName = layerName + "_style";
    registerStyle(styleName, *viewPtr->legend.get(), objectType, geomType);

    for(auto layer : layersArray)
      registerLayerDefaultStyle(styleName, layer.toObject().value("layer").toString().toStdString());
  }

  jsonAnswer.insert("workspace", QString::fromStdString(workspace()));
  jsonAnswer.insert("layers_list", layersArray);

  return jsonAnswer;
}


void terrama2::services::view::core::GeoServer::setWorkspace(const std::string& workspace)
{
  workspace_ = workspace;
}


const std::string& terrama2::services::view::core::GeoServer::getWorkspace(const std::string& name) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriPost(uri_.uri() + "/rest/workspaces/" + name);

  if(!uriPost.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriPost.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPost.uri()));
  }

  std::string responseWorkspace;

  // Register style
  cURLwrapper.get(uriPost, responseWorkspace);

  if(cURLwrapper.responseCode() == 404)
  {
    throw NotFoundGeoserverException() << ErrorDescription(QString::fromStdString(cURLwrapper.response()));
  }
  else if(cURLwrapper.responseCode() != 200)
  {
    QString errMsg = QObject::tr("Error at get Workspace. ");
    TERRAMA2_LOG_ERROR() << errMsg << uriPost.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(cURLwrapper.response()));
  }

  return cURLwrapper.response();
}


void terrama2::services::view::core::GeoServer::registerWorkspace(const std::string& name)
{
  if(!name.empty())
  {
    workspace_ = name;
  }

  try
  {
    getWorkspace(workspace_);
  }
  catch(NotFoundGeoserverException /*e*/)
  {
    te::ws::core::CurlWrapper cURLwrapper;

    te::core::URI uriPost(uri_.uri() + "/rest/workspaces");

    if(!uriPost.isValid())
    {
      QString errMsg = QObject::tr("Invalid URI.");
      TERRAMA2_LOG_ERROR() << errMsg << uriPost.uri();
      throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPost.uri()));
    }

    // Register style
    cURLwrapper.post(uriPost, "<workspace><name>" + workspace_ + "</name></workspace>", "Content-Type: text/xml");

    if(cURLwrapper.responseCode() != 201)
    {
      QString errMsg = QObject::tr("Error to register the workspace.");
      TERRAMA2_LOG_ERROR() << errMsg << cURLwrapper.response();
      throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(cURLwrapper.response()));
    }
  }
}

const std::string& terrama2::services::view::core::GeoServer::workspace() const
{
  return workspace_;
}

const std::string& terrama2::services::view::core::GeoServer::getDataStore(const std::string& name) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriGet(uri_.uri() + "/rest/workspaces/" + workspace_ + "/datastores/"
                       + QString(QUrl::toPercentEncoding(QString::fromStdString(name), "", "-._~/")).toStdString());

  if(!uriGet.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriGet.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriGet.uri()));
  }

  std::string responseDataStore;

  // Register style
  cURLwrapper.get(uriGet, responseDataStore);

  if(cURLwrapper.responseCode() == 404)
  {
    throw NotFoundGeoserverException() << ErrorDescription(QString::fromStdString(cURLwrapper.response()));
  }
  else if(cURLwrapper.responseCode() != 200)
  {
    QString errMsg = QObject::tr("Error at get Data Store. ");
    TERRAMA2_LOG_ERROR() << errMsg << uriGet.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(cURLwrapper.response()));
  }

  return cURLwrapper.response();
}


const std::string& terrama2::services::view::core::GeoServer::getCoverageStore(const std::string& name) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriGet(uri_.uri() + "/rest/workspaces/" + workspace_ + "/coveragestores/"
                       + QString(QUrl::toPercentEncoding(QString::fromStdString(name), "", "-._~/")).toStdString());

  if(!uriGet.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriGet.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriGet.uri()));
  }

  std::string responseDataStore;

  // Register style
  cURLwrapper.get(uriGet, responseDataStore);

  if(cURLwrapper.responseCode() == 404)
  {
    throw NotFoundGeoserverException() << ErrorDescription(QString::fromStdString(cURLwrapper.response()));
  }
  else if(cURLwrapper.responseCode() != 200)
  {
    QString errMsg = QObject::tr("Error at get Coverage Store. ");
    TERRAMA2_LOG_ERROR() << errMsg << uriGet.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(cURLwrapper.response()));
  }

  return cURLwrapper.response();
}


void terrama2::services::view::core::GeoServer::registerPostGisDataStore(const std::string& dataStoreName,
                                                                         const std::map<std::string, std::string> connInfo) const
{
  try
  {
    getDataStore(dataStoreName);
  }
  catch(NotFoundGeoserverException /*e*/)
  {
    te::ws::core::CurlWrapper cURLwrapper;

    te::core::URI uriPost(uri_.uri() + "/rest/workspaces/" + workspace_ + "/datastores");

    if(!uriPost.isValid())
    {
      QString errMsg = QObject::tr("Invalid URI.");
      TERRAMA2_LOG_ERROR() << errMsg << uriPost.uri();
      throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPost.uri()));
    }

    std::string xml = "<dataStore><name>" + dataStoreName +"</name>" +
                      "<connectionParameters>" +
                      "<host>" + connInfo.at("PG_HOST") + "</host>" +
                      "<port>" + connInfo.at("PG_PORT") +"</port>" +
                      "<database>" + connInfo.at("PG_DB_NAME") + "</database>" +
                      "<user>" + connInfo.at("PG_USER") + "</user>" +
                      "<passwd>" + connInfo.at("PG_PASSWORD") + "</passwd>" +
                      "<dbtype>postgis</dbtype>" +
                      "</connectionParameters>" +
                      "</dataStore>";

    // Register data store
    cURLwrapper.post(uriPost, xml, "Content-Type: text/xml");
  }
}


void terrama2::services::view::core::GeoServer::registerVectorDataStore(const std::string& dataStoreName,
                                                                        const std::string& shpFilePath) const
{
  try
  {
    getDataStore(dataStoreName);
  }
  catch(NotFoundGeoserverException /*e*/)
  {
    te::ws::core::CurlWrapper cURLwrapper;

    te::core::URI uriPostDatastore(uri_.uri() +"/rest/workspaces/" + workspace_ + "/datastores.xml");

    if(!uriPostDatastore.isValid())
    {
      QString errMsg = QObject::tr("Invalid URI.");
      TERRAMA2_LOG_ERROR() << errMsg << uriPostDatastore.uri();
      throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPostDatastore.uri()));
    }

    std::string xml = "<dataStore>";
    xml += "<name>" + dataStoreName + "</name>";
    xml += "<type>Shapefile</type>";
    xml += "<enabled>true</enabled>";

    xml += "<connectionParameters>"
           "<entry key=\"charset\">ISO-8859-1</entry>"
           "<entry key=\"filetype\">shapefile</entry>"
           "<entry key=\"create spatial index\">true</entry>"
           "<entry key=\"memory mapped buffer\">false</entry>"
           "<entry key=\"enable spatial index\">true</entry>"
           "<entry key=\"cache and reuse memory maps\">true</entry>";
    xml += "<entry key=\"url\">";
    xml += "file://" + shpFilePath;
    xml += "</entry>";
    xml += "</connectionParameters>";

    xml += "</dataStore>";

    cURLwrapper.post(uriPostDatastore, xml, "Content-Type: text/xml");

    if(cURLwrapper.responseCode() != 201)
    {
      QString errMsg = QObject::tr("Error at register vectorial data store. ");
      TERRAMA2_LOG_ERROR() << errMsg << uriPostDatastore.uri();
      throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(cURLwrapper.response()));
    }
  }
}


const std::string& terrama2::services::view::core::GeoServer::getFeature(const std::string& dataStoreName,
                                                                         const std::string& name) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriGet(uri_.uri() + "/rest/workspaces/" + workspace_ + "/datastores/"
                       +  QString(QUrl::toPercentEncoding(QString::fromStdString(dataStoreName), "", "-._~/")).toStdString()
                       + "/featuretypes/"
                       + QString(QUrl::toPercentEncoding(QString::fromStdString(name), "", "-._~/")).toStdString());

  if(!uriGet.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriGet.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriGet.uri()));
  }

  std::string responseDataStore;

  cURLwrapper.get(uriGet, responseDataStore);

  if(cURLwrapper.responseCode() == 404)
  {
    throw NotFoundGeoserverException() << ErrorDescription(QString::fromStdString(cURLwrapper.response()));
  }
  else if(cURLwrapper.responseCode() != 200)
  {
    QString errMsg = QObject::tr("Error at get Feature. ");
    TERRAMA2_LOG_ERROR() << errMsg << uriGet.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(cURLwrapper.response()));
  }

  return cURLwrapper.response();
}


void terrama2::services::view::core::GeoServer::registerPostgisTable(const ViewPtr viewPtr,
                                                                     const std::string& dataStoreName,
                                                                     terrama2::core::DataSeriesType dataSeriesType,
                                                                     std::map<std::string, std::string> connInfo,
                                                                     const std::string& tableName,
                                                                     const std::string& layerName,
                                                                     const std::unique_ptr<te::da::DataSetType>& dataSetType,
                                                                     const std::string& timestampPropertyName,
                                                                     const std::string& sql) const
{
  try
  {
    deleteVectorLayer(dataStoreName, layerName, true);
  }
  catch(NotFoundGeoserverException /*e*/)
  {
    // Do nothing
  }

  registerPostGisDataStore(dataStoreName, connInfo);

  te::ws::core::CurlWrapper cURLwrapper;

  std::string xml = "<featureType>";
  xml += "<title>" + viewLayerTitle(viewPtr) + "</title>";
  xml += "<name>" + viewLayerName(viewPtr) + "</name>";
  if(dataSeriesType == terrama2::core::DataSeriesType::ANALYSIS_MONITORED_OBJECT
     || dataSeriesType == terrama2::core::DataSeriesType::DCP)
    xml += "<nativeName>" + layerName + "</nativeName>";
  else
  xml += "<nativeName>" + tableName + "</nativeName>";
  xml += "<enabled>true</enabled>";

  std::string metadataTime = "";
  std::string metadataSQL = "";

  std::string presentation;
  if(dataSeriesType == terrama2::core::DataSeriesType::ANALYSIS_MONITORED_OBJECT)
    presentation = "LIST";
  else
    presentation = "CONTINUOUS_INTERVAL";

  if(!timestampPropertyName.empty())
  {
    metadataTime = "<entry key=\"time\">"
                   "<dimensionInfo>"
                   "<enabled>true</enabled>"
                   "<attribute>"+timestampPropertyName+"</attribute>"+
                   "<presentation>"+presentation+"</presentation>"
                   "<units>ISO8601</units>"
                   "<defaultValue>"
                   "<strategy>MAXIMUM</strategy>"
                   "</defaultValue>"
                   "</dimensionInfo>"
                   "</entry>"
                   "<entry key=\"cachingEnabled\">false</entry>";

  }

  if(!sql.empty())
  {
    std::string geomName;
    te::gm::GeomType geomType;
    std::string srid;

    if(dataSetType && dataSetType->hasGeom())
    {
      auto geomProperty = te::da::GetFirstGeomProperty(dataSetType.get());
      geomName = geomProperty->getName();
      geomType = geomProperty->getGeometryType();
      srid = std::to_string(geomProperty->getSRID());
    }

    // Configuring SRID on Root XML configuration
    xml += "<srs>EPSG:" + srid + "</srs>";

    metadataSQL = "<entry key='JDBC_VIRTUAL_TABLE'>"
                  "<virtualTable>"
                  "<name>"+layerName+"</name>" +
                  "<sql>"+sql+"</sql>" +
                  "<escapeSql>false</escapeSql>";

    if(!geomName.empty())
    {
      metadataSQL += "<geometry>"
                     "<name>"+geomName+"</name>" +
                     "<type>"+getGeomTypeString(geomType)+"</type>" +
                     "<srid>"+srid+"</srid>" +
                     "</geometry>";
    }

    metadataSQL += "</virtualTable>"
                   "</entry>";
  }

  if(!metadataTime.empty() || !metadataSQL.empty())
  {
    xml += "<metadata>" + metadataTime + metadataSQL + "</metadata>";
  }

  xml += "</featureType>";

  std::string uri = uri_.uri() + "/rest/workspaces/" + workspace_ + "/datastores/"
                    + QString(QUrl::toPercentEncoding(QString::fromStdString(dataStoreName), "", "-._~/")).toStdString()
                    +"/featuretypes";

  te::core::URI uriPostLayer(uri);
  cURLwrapper.post(uriPostLayer, xml, "Content-Type: text/xml");

  if(cURLwrapper.responseCode() != 201)
  {
    QString errMsg = QObject::tr("Error at register PostGis Table. ");
    TERRAMA2_LOG_ERROR() << errMsg << uriPostLayer.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(cURLwrapper.response()));
  }
}


void terrama2::services::view::core::GeoServer::uploadZipVectorFiles(const std::string& dataStoreName,
                                                                     const std::string& shpZipFilePath,
                                                                     const std::string& extension) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriPut(uri_.uri() + "/rest/workspaces/" + workspace_ + "/datastores/"
                       + QString(QUrl::toPercentEncoding(QString::fromStdString(dataStoreName), "", "-._~/")).toStdString()
                       + "/file." + extension + "?configure=all&update=append");

  if(!uriPut.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriPut.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPut.uri()));
  }
  // Upload Vector file
  cURLwrapper.putFile(uriPut, shpZipFilePath, "Content-type: application/zip");
}


void terrama2::services::view::core::GeoServer::registerVectorFile(const ViewPtr& viewPtr,
                                                                   const std::string& dataStoreName,
                                                                   const std::string& shpFilePath,
                                                                   const std::string& layerName) const
{
  try
  {
    deleteVectorLayer(dataStoreName, layerName, true);
  }
  catch(const NotFoundGeoserverException&)
  {
    // Do nothing
  }

  registerVectorDataStore(dataStoreName, shpFilePath);

  te::ws::core::CurlWrapper cURLwrapper;

  std::string uri = uri_.uri() + "/rest/workspaces/" + workspace_ + "/datastores/"
                    + QString(QUrl::toPercentEncoding(QString::fromStdString(dataStoreName), "", "-._~/")).toStdString() +"/featuretypes";

  te::core::URI uriPost(uri);

  if(!uriPost.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriPost.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPost.uri()));
  }

  std::string xmlFeature = "<featureType>";
  xmlFeature += "<title>" + viewLayerTitle(viewPtr) + "</title>";
  xmlFeature += "<name>" + viewLayerName(viewPtr) + "</name>";

  QFileInfo file(QString::fromStdString(shpFilePath));
  xmlFeature += "<nativeName>" + file.baseName().toStdString() + "</nativeName>";
  xmlFeature += "<enabled>true</enabled>";
  xmlFeature += "</featureType>";

  cURLwrapper.post(uriPost, xmlFeature, "Content-Type: text/xml");

  if(cURLwrapper.responseCode() != 201)
  {
    QString errMsg = QObject::tr("Error at register vectorial file. ");
    TERRAMA2_LOG_ERROR() << errMsg << uriPost.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(cURLwrapper.response()));
  }
}


void terrama2::services::view::core::GeoServer::registerVectorsFolder(const std::string &dataStoreName,
                                                                      const std::string &shpFolderPath,
                                                                      const std::string &extension) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriPut(uri_.uri() + "/rest/workspaces/" + workspace_ + "/datastores/"
                       + QString(QUrl::toPercentEncoding(QString::fromStdString(dataStoreName), "", "-._~/")).toStdString()
                       + "/external." + extension + "?configure=all&update=append");

  if(!uriPut.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriPut.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPut.uri()));
  }

  // Register Vector file
  cURLwrapper.customRequest(uriPut, "PUT", "file://" + shpFolderPath);
}


void terrama2::services::view::core::GeoServer::uploadZipCoverageFile(const std::string& coverageStoreName,
                                                                      const std::string& coverageZipFilePath,
                                                                      const std::string& extension) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriPut(uri_.uri() + "/rest/workspaces/" + workspace_ + "/coveragestores/"
                       + QString(QUrl::toPercentEncoding(QString::fromStdString(coverageStoreName), "", "-._~/")).toStdString()
                       + "/file." + extension + "?configure=all");

  if(!uriPut.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriPut.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPut.uri()));
  }
  // Upload Coverage file
  cURLwrapper.putFile(uriPut, coverageZipFilePath, "Content-type: application/zip");
}


void terrama2::services::view::core::GeoServer::registerCoverageFile(const std::string& coverageStoreName,
                                                                     const std::string& coverageFilePath,
                                                                     const std::string& coverageName,
                                                                     const std::string& extension) const
{
  try
  {
    deleteCoverageLayer(coverageStoreName, coverageName, true);
  }
  catch(NotFoundGeoserverException /*e*/)
  {
    // Do nothing
  }

  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriPut(uri_.uri() + "/rest/workspaces/" + workspace_ + "/coveragestores/"
                       + QString(QUrl::toPercentEncoding(QString::fromStdString(coverageStoreName), "", "-._~/")).toStdString()
                       + "/external." + extension + "?configure=first&coverageName=" + coverageName);

  if(!uriPut.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriPut.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPut.uri()));
  }
  // Upload Coverage file
  cURLwrapper.customRequest(uriPut, "PUT", "file://" + coverageFilePath);
}


void terrama2::services::view::core::GeoServer::registerMosaicCoverage(const ViewPtr viewPtr,
                                                                       const std::string& coverageStoreName,
                                                                       const std::string& mosaicPath,
                                                                       const std::string& coverageName,
                                                                       const RasterInfo& rasterInfo,
                                                                       const std::string& style,
                                                                       const std::string& configure) const
{
  std::string storeName = QString(QUrl::toPercentEncoding(QString::fromStdString(coverageStoreName), "", "-._~/")).toStdString();

  try
  {
    getCoverageStore(storeName);
  }
  catch(const NotFoundGeoserverException& /*e*/)
  {
    te::ws::core::CurlWrapper cURLwrapper;

    te::core::URI uriPut(uri_.uri() + "/rest/workspaces/" + workspace_ + "/coveragestores/"
                         + storeName
                         + "/external.imagemosaic?configure=" + configure);

    if(!uriPut.isValid())
    {
      QString errMsg = QObject::tr("Invalid URI.");
      TERRAMA2_LOG_ERROR() << errMsg << uriPut.uri();
      throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPut.uri()));
    }

    std::string path = "file://" + mosaicPath;

    // Upload Coverage file
    cURLwrapper.customRequest(uriPut, "PUT", path, "Content-Type: text/plain");

    if(cURLwrapper.responseCode() != 201)
    {
      QString errMsg = QObject::tr(cURLwrapper.response().c_str());
      TERRAMA2_LOG_ERROR() << errMsg << uriPut.uri();
      throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPut.uri()));
    }

    te::core::URI uriPutUpdateCoverage(uri_.uri() + "/rest/workspaces/" + workspace_ + "/coveragestores/"
                                       + storeName
                                       + "/coverages/" + coverageName);

    std::string xml = "<coverage>";
    xml += "<title>" + viewLayerTitle(viewPtr) + "</title>";
    xml += "<name>" + viewLayerName(viewPtr) + "</name>";

    // Using TerraMA² toString to keep precision
    const std::string llx = toString(rasterInfo.envelope->getLowerLeftX(), 14);
    const std::string urx = toString(rasterInfo.envelope->getUpperRightX(), 14);
    const std::string lly = toString(rasterInfo.envelope->getLowerLeftY(), 14);
    const std::string ury = toString(rasterInfo.envelope->getUpperRightY(), 14);
    const std::string srid = std::to_string(rasterInfo.srid);
    // Settings bounds
    xml += "<nativeBoundingBox>"
           "  <minx>"+llx+"</minx>"
           "  <maxx>"+urx+"</maxx>"
           "  <miny>"+lly+"</miny>"
           "  <maxy>"+ury+"</maxy>"
           "  <crs>EPSG:"+srid+"</crs>"
           "</nativeBoundingBox>";

    // Settingd grid
    xml += "<latLonBoundingBox>"
           "  <minx>"+llx+"</minx>"
           "  <maxx>"+urx+"</maxx>"
           "  <miny>"+lly+"</miny>"
           "  <maxy>"+ury+"</maxy>"
           "  <crs>EPSG:"+srid+"</crs>"
           "</latLonBoundingBox>";

    xml += "<enabled>true</enabled>"
           "  <srs>EPSG:"+
           std::to_string(rasterInfo.srid) +
           "  </srs>"
           "  <metadata>"
           "  <entry key=\"time\">"
           "  <dimensionInfo>"
           "  <enabled>true</enabled>"
           "<presentation>LIST</presentation>"
           "<units>ISO8601</units>"
           "<defaultValue>"
           "<strategy>MAXIMUM</strategy>"
           "</defaultValue>"
           "</dimensionInfo>"
           "</entry>"
           "</metadata>"
           "</coverage>";

    cURLwrapper.customRequest(uriPutUpdateCoverage, "PUT", xml, "Content-Type: text/xml");

    if(cURLwrapper.responseCode() != 200)
    {
      QString errMsg = QObject::tr(cURLwrapper.response().c_str());
      TERRAMA2_LOG_ERROR() << errMsg << uriPutUpdateCoverage.uri();
      throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPut.uri()));
    }

    if(!style.empty())
    {
      te::core::URI layerStyle(uri_.uri() + "/rest/layers/" + coverageName + ".xml");

      cURLwrapper.customRequest(layerStyle, "PUT",
                                "<layer><defaultStyle><name>" + style + "</name><workspace>" + workspace_ + "</workspace></defaultStyle></layer>", "Content-Type: text/xml");
    }
  }
}


void terrama2::services::view::core::GeoServer::registerStyleFile(const std::string& name,
                                                                  const std::string& styleFilePath) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriPost(uri_.uri() + "/rest/workspaces/" + workspace_ + "/styles");

  if(!uriPost.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriPost.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPost.uri()));
  }

  // Register style
  cURLwrapper.post(uriPost, "<style><name>" + name + "</name><filename>" + name + ".sld</filename></style>", "Content-Type: text/xml");


  te::core::URI uriPut(uri_.uri() + "/rest/workspaces/" + workspace_ + "/styles/" + name +"?raw=true");

  if(!uriPut.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriPut.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPut.uri()));
  }

  // Upload Style file
  cURLwrapper.putFile(uriPut, styleFilePath, "Content-type: application/vnd.ogc.se+xml");
}


void terrama2::services::view::core::GeoServer::registerStyle(const std::string &name,
                                                              const std::unique_ptr<te::se::Style> &style) const
{
  QTemporaryFile file;
  if(!file.open())
  {
    QString errMsg = QObject::tr("Can't open the file.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ViewGeoserverException() << ErrorDescription(errMsg);
  }

  std::string filePath = file.fileName().toStdString();

  Serialization::writeVectorialStyleGeoserverXML(style.get(), filePath);

  QByteArray content = file.readAll();
  if(content.isEmpty())
  {
    QString errMsg = QObject::tr("Can't read the SLD file.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ViewGeoserverException() << ErrorDescription(errMsg);
  }

  registerStyle(name, QString(content).toStdString());
}


void terrama2::services::view::core::GeoServer::registerStyle(const std::string& name,
                                                              const std::string &style,
                                                              const std::string& sldVersion) const
{
  std::string validName = QString(QUrl::toPercentEncoding(QString::fromStdString(name), "", "-._~/")).toStdString();

  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriPost(uri_.uri() + "/rest/workspaces/" + workspace_ + "/styles?name="
                        + validName
                        + "&raw=true");

  if(!uriPost.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriPost.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPost.uri()));
  }

  std::string contentType = "Content-Type: application/vnd.ogc.se+xml";

  if(sldVersion == "1.0.0")
  {
    contentType = "Content-Type: application/vnd.ogc.sld+xml";
  }

  // Register style
  cURLwrapper.post(uriPost, style, contentType);

  if(cURLwrapper.responseCode() == 403)
  {
    te::core::URI uriPut(uri_.uri() + "/rest/workspaces/" + workspace_ + "/styles/" + validName +"?raw=true");

    if(!uriPut.isValid())
    {
      QString errMsg = QObject::tr("Invalid URI.");
      TERRAMA2_LOG_ERROR() << errMsg << uriPut.uri();
      throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPut.uri()));
    }

    cURLwrapper.customRequest(uriPut, "PUT", style, "Content-Type: application/vnd.ogc.se+xml");

    if(cURLwrapper.responseCode() != 200)
    {
      QString errMsg = QObject::tr(cURLwrapper.response().c_str());
      TERRAMA2_LOG_ERROR() << errMsg << uriPost.uri();
      throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(cURLwrapper.response()));
    }
  }
  else if(cURLwrapper.responseCode() != 201)
  {
    QString errMsg = QObject::tr(cURLwrapper.response().c_str());
    TERRAMA2_LOG_ERROR() << errMsg << uriPost.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(cURLwrapper.response()));
  }
}


void terrama2::services::view::core::GeoServer::registerStyle(const std::string& name,
                                                              const View::Legend& legend,
                                                              const View::Legend::ObjectType& objectType,
                                                              const te::gm::GeomType& geomType) const
{
  if(objectType == View::Legend::ObjectType::GEOMETRY)
  {
    if(legend.metadata.at("creation_type") != "editor")
    {
      registerStyle(name, legend.metadata.at("xml_style"));

      return;
    }

    std::unique_ptr<te::se::Style> style(generateVectorialStyle(legend, geomType).release());

    registerStyle(name, style);
  }
  else if(objectType == View::Legend::ObjectType::RASTER)
  {
    if(legend.metadata.at("creation_type") != "editor")
    {
      registerStyle(name, legend.metadata.at("xml_style"), "1.0.0");

      return;
    }

    QTemporaryFile file;
    if(!file.open())
    {
      QString errMsg = QObject::tr("Can't open the file.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw ViewGeoserverException() << ErrorDescription(errMsg);
    }

    std::string filePath = file.fileName().toStdString();

    Serialization::writeCoverageStyleGeoserverXML(legend, filePath);

    QByteArray content = file.readAll();
    if(content.isEmpty())
    {
      QString errMsg = QObject::tr("Can't read the SLD file.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw ViewGeoserverException() << ErrorDescription(errMsg);
    }

    registerStyle(name, QString(content).toStdString(), "1.0.0");
  }
  else
  {
    QString errMsg = QObject::tr("Can't generate style for Layer.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ViewGeoserverException() << ErrorDescription(errMsg);
  }
}


std::unique_ptr<te::se::Style> terrama2::services::view::core::GeoServer::generateVectorialStyle(const View::Legend& legend,
                                                                                                 const te::gm::GeomType& geomType) const
{
  std::unique_ptr<te::se::Style> style(new te::se::FeatureTypeStyle());

  std::vector<View::Legend::Rule> legendRules = legend.rules;

  if(legend.operation == View::Legend::OperationType::VALUE)
  {
    if(legend.classify == View::Legend::ClassifyType::INTERVALS)
    {
      std::sort(legendRules.begin(), legendRules.end(), View::Legend::Rule::compareByNumericValue);
    }
  }

  if(legend.operation == View::Legend::OperationType::VALUE)
  {
    std::vector<std::unique_ptr<te::se::Rule> > rules;
    std::unique_ptr<te::se::Rule> ruleDefault;

    for(std::size_t i = 0; i < legendRules.size(); ++i)
    {
      auto legendRule = legendRules[i];
      std::unique_ptr<te::se::Symbolizer> symbolizer(getSymbolizer(geomType, legendRule.color, legendRule.opacity));

      std::unique_ptr<te::se::Rule> rule(new te::se::Rule);
      rule->push_back(symbolizer.release());
      rule->setName(new std::string(legendRule.title));

      if(legendRule.isDefault)
      {
        ruleDefault = std::move(rule);
        continue;
      }

      std::unique_ptr<te::fe::PropertyName> propertyName (new te::fe::PropertyName(getAttributeName(legend)));
      std::unique_ptr<te::fe::Literal> value (new te::fe::Literal(legendRule.value));

      // Defining OGC Style Filter
      std::unique_ptr<te::fe::Filter> filter(new te::fe::Filter);

      if(legend.classify == View::Legend::ClassifyType::VALUES)
        filter->setOp(new te::fe::BinaryComparisonOp(te::fe::Globals::sm_propertyIsEqualTo, propertyName->clone(), value->clone()));
      else if(legend.classify == View::Legend::ClassifyType::INTERVALS)
      {
        // If second node, use LessThanOrEqualTo operator (<=)
        if (i == 1)
          filter->setOp(new te::fe::BinaryComparisonOp(te::fe::Globals::sm_propertyIsLessThanOrEqualTo, propertyName->clone(),value->clone()));
        else if (i < legendRules.size())
        {
          // For next nodes, make interval AND with GreaterThan and LessThanOrEqualTo
          // to avoid GeoServer merge colors while re-painting

          // Retrieve prior rule value to use as initial value
          std::unique_ptr<te::fe::Literal> priorValue (new te::fe::Literal(legendRules[i-1].value));

          // Making a check for values greater than last legend
          std::unique_ptr<te::fe::BinaryComparisonOp> minOperator (new te::fe::BinaryComparisonOp(te::fe::Globals::sm_propertyIsGreaterThan,
                                                                                   propertyName->clone(),
                                                                                   priorValue->clone()));

          // Making a check for values less than or equal to current rule
          std::unique_ptr<te::fe::BinaryComparisonOp> maxOperator (new te::fe::BinaryComparisonOp(te::fe::Globals::sm_propertyIsLessThanOrEqualTo,
                                                                                   propertyName->clone(),
                                                                                   value->clone()));

          filter->setOp(new te::fe::And(minOperator.release(), maxOperator.release()));
        }
      }

      if(filter->getOp() == nullptr)
      {
        QString errMsg = QObject::tr("Unknown classification type!");
        TERRAMA2_LOG_ERROR() << errMsg;
        throw ViewGeoserverException() << ErrorDescription(errMsg);
      }

      rule->setFilter(filter.release());

      rules.push_back(std::move(rule));
    }

    if(ruleDefault)
      style->push_back(ruleDefault.release());

    for(auto& rule : rules)
    {
      if(rule)
        style->push_back(rule.release());
    }
  }
  else if(legend.operation == View::Legend::OperationType::EQUAL_STEPS)
  {
    // TODO:
    QString errMsg = QObject::tr("Not Implemented!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ViewGeoserverException() << ErrorDescription(errMsg);
  }
  else if(legend.operation == View::Legend::OperationType::QUANTIL)
  {
    // TODO:
    QString errMsg = QObject::tr("Not Implemented!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ViewGeoserverException() << ErrorDescription(errMsg);
  }
  else
  {
    QString errMsg = QObject::tr("Unknow Operation!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ViewGeoserverException() << ErrorDescription(errMsg);
  }

  return style;
}

void terrama2::services::view::core::GeoServer::cleanup(const ViewPtr& viewPtr,
                                                        terrama2::core::DataProviderPtr dataProvider,
                                                        std::shared_ptr<terrama2::core::ProcessLogger> logger)
{
  std::string workspace_to_remove = workspace_;
  if (viewPtr != nullptr && logger != nullptr)
  {
    ViewId id = viewPtr->id;
    workspace_to_remove = workspaceName(viewPtr);

    // Try to remove cached view table
    const std::string& tableName = viewLayerName(viewPtr);
    // Removing view table
    try
    {
      TERRAMA2_LOG_DEBUG() << "Removing " + tableName;
      removeTable(tableName, logger->getConnectionInfo());
    }
    catch(...)
    {
      TERRAMA2_LOG_DEBUG() << "Could not remove view table " + tableName;
    }

    // Performs disk files removal only for data providers FILE
    if (dataProvider != nullptr && dataProvider->dataProviderType == "FILE")
    {
      QUrl uri((dataProvider->uri+ "/" + tableName).c_str());
      // If provided URI is empty, skip it
      if (!uri.toLocalFile().toStdString().empty())
      {
        QDir directory(uri.toLocalFile());

        // Extra validations to skip "." and ".."
        directory.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

        // Counting number of files. If only 2, might to be only *.properties files
        const int numberFiles = directory.count();

        // Retrieve files that match filter "*.properties"
        QStringList filters;
        filters << "*.properties";

        const QStringList files = directory.entryList(filters);

        // If exactly 2, remove folder
        if ((numberFiles == files.size()) && (numberFiles == 2))
          removeFolder(uri.toLocalFile().toStdString());
        else
        {
          // If matched files exist
          if (files.size() > 0)
          {
            /*
             * It should never happen since we are working with unique view names (view + viewID)
             * But the user can manually rename/remove folder or even put new files inside. TODO: Should notify a warn?
             * We must ensure that only properties files will be removed.
             */
            std::string warningMessage = "The directory \""+ uri.toLocalFile().toStdString() + "\" is not empty. " +
                                         "The following files has been removed: ";
            for(const auto& fileName: files)
            {
              const std::string fileURI = uri.toLocalFile().toStdString() + "/" + fileName.toStdString();
              removeFile(fileURI);
              warningMessage += "\n  -" + fileURI;
            }

            logger->log(terrama2::core::ProcessLogger::WARNING_MESSAGE, warningMessage, id);
          }
        }
      } // endif !directory.currentPath().toStdString().empty()
    } // endif (dataProvider != nullptr && dataProvider->dataProviderType == "FILE")
  }

  te::ws::core::CurlWrapper curl;

  // Generating Rest Workspace URI to force removal of all layers, data stores and so on.
  std::string url = "/rest/workspaces/" + workspace_to_remove+ "?recurse=true";

  te::core::URI uriDelete(uri_.uri() + url);

  if(!uriDelete.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriDelete.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriDelete.uri()));
  }

  curl.customRequest(uriDelete, "DELETE");

  switch(curl.responseCode())
  {
    case 200:
      break; // Successfully
    case 404:
      throw NotFoundGeoserverException() << ErrorDescription(QString::fromStdString(curl.response()));
    default:
      QString errMsg = QObject::tr("Error at delete Workspace. ");
      TERRAMA2_LOG_ERROR() << errMsg << uriDelete.uri();
      throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(curl.response()));
  }
}

void terrama2::services::view::core::GeoServer::deleteVectorLayer(const std::string& dataStoreName,
                                                                  const std::string &fileName,
                                                                  bool recursive) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  std::string url = "/rest/workspaces/" + workspace_ + "/datastores/"
                    + QString(QUrl::toPercentEncoding(QString::fromStdString(dataStoreName), "", "-._~/")).toStdString()
                    + "/featuretypes/"
                    + QString(QUrl::toPercentEncoding(QString::fromStdString(fileName), "", "-._~/")).toStdString();

  if(recursive)
  {
    url += "?recurse=true";
  }

  te::core::URI uriDelete(uri_.uri() + url);

  if(!uriDelete.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriDelete.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriDelete.uri()));
  }

  cURLwrapper.customRequest(uriDelete, "DELETE");

  if(cURLwrapper.responseCode() == 404)
  {
    throw NotFoundGeoserverException() << ErrorDescription(QString::fromStdString(cURLwrapper.response()));
  }
  else if(cURLwrapper.responseCode() != 200)
  {
    QString errMsg = QObject::tr("Error at delete Vectorial Layer. ");
    TERRAMA2_LOG_ERROR() << errMsg << uriDelete.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(cURLwrapper.response()));
  }
}


void terrama2::services::view::core::GeoServer::deleteCoverageLayer(const std::string& coverageStoreName,
                                                                    const std::string& fileName,
                                                                    bool recursive) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  std::string url = "/rest/workspaces/" + workspace_ + "/coveragestores/"
                    + QString(QUrl::toPercentEncoding(QString::fromStdString(coverageStoreName), "", "-._~/")).toStdString()
                    + "/coverages/"
                    + QString(QUrl::toPercentEncoding(QString::fromStdString(fileName), "", "-._~/")).toStdString();

  if(recursive)
  {
    url += "?recurse=true";
  }

  te::core::URI uriDelete(uri_.uri() + url);

  if(!uriDelete.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriDelete.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriDelete.uri()));
  }

  cURLwrapper.customRequest(uriDelete, "DELETE");

  if(cURLwrapper.responseCode() == 404)
  {
    throw NotFoundGeoserverException() << ErrorDescription(QString::fromStdString(cURLwrapper.response()));
  }
  else if(cURLwrapper.responseCode() != 200)
  {
    QString errMsg = QObject::tr("Error at delete Coverage Layer. ");
    TERRAMA2_LOG_ERROR() << errMsg << uriDelete.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(cURLwrapper.response()));
  }
}


void terrama2::services::view::core::GeoServer::deleteStyle(const std::string& styleName) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriDelete(uri_.uri() + "/rest/workspaces/" + workspace_ + "/styles/"
                          + QString(QUrl::toPercentEncoding(QString::fromStdString(styleName), "", "-._~/")).toStdString() + "?purge=true");

  if(!uriDelete.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriDelete.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriDelete.uri()));
  }

  cURLwrapper.customRequest(uriDelete, "DELETE");

  if(cURLwrapper.responseCode() == 404)
  {
    throw NotFoundGeoserverException() << ErrorDescription(QString::fromStdString(cURLwrapper.response()));
  }
  else if(cURLwrapper.responseCode() != 200)
  {
    QString errMsg = QObject::tr("Error at delete style. ");
    TERRAMA2_LOG_ERROR() << errMsg << uriDelete.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(cURLwrapper.response()));
  }
}


void terrama2::services::view::core::GeoServer::getMapWMS(const std::string& savePath,
                                                          const std::string& fileName,
                                                          const std::list<std::pair<std::string, std::string>> layersAndStyles,
                                                          const te::gm::Envelope env,
                                                          const uint32_t width,
                                                          const uint32_t height,
                                                          const uint32_t srid,
                                                          const std::string& format) const
{
  if(layersAndStyles.empty())
  {
    QString errMsg = QObject::tr("No layers defined.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ViewGeoserverException() << ErrorDescription(errMsg);
  }

  std::string version = "1.1.0";

  te::ws::ogc::WMSClient wms(savePath, uri_.uri(), version);

  std::string request = (uri_.uri() + "/wms?service=WMS" +
                         "&version=" + version +
                         "&request=GetMap");

  std::string layers = "&layers=";
  std::string styles = "&styles=";

  for(auto it = layersAndStyles.begin(); it != layersAndStyles.end(); ++it)
  {
    if(it == layersAndStyles.begin())
    {
      layers += it->first;
      styles += it->second;
    }
    else
    {
      layers += "," + it->first;
      styles += "," + it->second;
    }
  }

  request += layers;
  request += styles;
  request += "&bbox=" + std::to_string(env.getLowerLeftX()) + "," + std::to_string(env.getLowerLeftY()) + "," + std::to_string(env.getUpperRightX()) + "," + std::to_string(env.getUpperRightY());
  request += "&width=" + std::to_string(width);
  request += "&height=" + std::to_string(height);
  request += "&srs=EPSG:" + std::to_string(srid);
  request += "&format=" + format;

  te::core::URI uriRequest(request);

  wms.makeFileRequest(uriRequest.uri(), fileName);
}


terrama2::services::view::core::MapsServerPtr terrama2::services::view::core::GeoServer::make(te::core::URI uri)
{
  return std::make_shared<GeoServer>(uri);
}


void terrama2::services::view::core::GeoServer::registerLayerDefaultStyle(const std::string& styleName,
                                                                          const std::string& layerName) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriPut(uri_.uri() + "/rest/layers/" + layerName + ".xml");

  if(!uriPut.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriPut.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPut.uri()));
  }

  cURLwrapper.customRequest(uriPut, "PUT",
                            "<layer><defaultStyle><name>" + styleName + "</name><workspace>" + workspace_ + "</workspace></defaultStyle></layer>", "Content-Type: text/xml");
}


std::string terrama2::services::view::core::GeoServer::getGeomTypeString(const te::gm::GeomType& geomType) const
{
  switch(geomType)
  {
    case te::gm::GeomType::MultiPolygonType :
    case te::gm::GeomType::MultiPolygonZType :
    case te::gm::GeomType::MultiPolygonMType :
    case te::gm::GeomType::MultiPolygonZMType :
      return "MultiPolygon";
    case te::gm::GeomType::GeometryType :
    case te::gm::GeomType::GeometryZType :
    case te::gm::GeomType::GeometryMType :
    case te::gm::GeomType::GeometryZMType :
      return "Geometry";
    case te::gm::GeomType::PointType :
    case te::gm::GeomType::PointZType :
    case te::gm::GeomType::PointMType :
    case te::gm::GeomType::PointZMType :
    case te::gm::GeomType::PointKdType :
      return "Point";
    case te::gm::GeomType::MultiPointType :
    case te::gm::GeomType::MultiPointZType :
    case te::gm::GeomType::MultiPointMType :
    case te::gm::GeomType::MultiPointZMType :
      return "MultiPoint";
    case te::gm::GeomType::LineStringType :
    case te::gm::GeomType::LineStringZType :
    case te::gm::GeomType::LineStringMType :
    case te::gm::GeomType::LineStringZMType :
      return "LineString";
    case te::gm::GeomType::MultiLineStringType :
    case te::gm::GeomType::MultiLineStringZType :
    case te::gm::GeomType::MultiLineStringMType :
    case te::gm::GeomType::MultiLineStringZMType :
      return "MultiLineString";
    case te::gm::GeomType::PolygonType :
    case te::gm::GeomType::PolygonZType :
    case te::gm::GeomType::PolygonMType :
    case te::gm::GeomType::PolygonZMType :
      return "Polygon";
    default:
      QString errMsg = QObject::tr("Unknown geometry type. ");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw ViewGeoserverException() << ErrorDescription(errMsg);
  }
}

std::vector<std::string> terrama2::services::view::core::GeoServer::registerMosaics(const terrama2::core::DataProviderPtr inputDataProvider,
                                                                                    const terrama2::core::DataSeriesPtr inputDataSeries,
                                                                                    const std::shared_ptr<DataManager> dataManager,
                                                                                    const ViewPtr viewPtr,
                                                                                    const te::core::URI& connInfo) const
{
  std::shared_ptr< te::da::DataSource > dataSource(te::da::DataSourceFactory::make("POSTGIS", connInfo));
  terrama2::core::OpenClose<std::shared_ptr<te::da::DataSource>> openClose(dataSource);
  if(!dataSource->isOpened())
  {
    QString errMsg = QObject::tr("Could not connect to database");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw Exception() << ErrorDescription(errMsg);
  }

  QUrl baseUrl(QString::fromStdString(inputDataProvider->uri));

  std::vector<std::string> layersNames;

  for(auto& dataset : inputDataSeries->datasetList)
  {
    std::string layerName = viewLayerName(viewPtr);
    std::transform(layerName.begin(), layerName.end(),layerName.begin(), ::tolower);

    QUrl url(baseUrl.toString() + QString::fromStdString("/" + terrama2::core::getFolderMask(dataset) + "/" + layerName));

    auto vecRasterInfo = getRasterInfo(dataManager, dataset, viewPtr->filter);

    int srid = vecRasterInfo[0].srid;

    if(!dataSource->dataSetExists(layerName))
    {
      // create table
      createMosaicTable(dataSource, layerName, srid);
    }

    // get all dates stored in the dataset
    std::vector<std::shared_ptr<te::dt::DateTime> > vecDates;

    {
      if(!dataSource->dataSetExists(layerName))
      {
        QString errMsg = QObject::tr("Could not find dataset!");
        TERRAMA2_LOG_ERROR() << errMsg;
        throw Exception() << ErrorDescription(errMsg);
      }

      std::unique_ptr<te::da::DataSet> teDataSet(dataSource->getDataSet(layerName));

      vecDates = terrama2::core::getAllDates(teDataSet.get(), "timestamp");
    }

    std::unique_ptr<te::da::DataSetType> teDataSetType(dataSource->getDataSetType(layerName));

    auto primaryKey = teDataSetType->getPrimaryKey();
    if(primaryKey != nullptr)
    {
      auto vecPkProperties = primaryKey->getProperties();

      for(auto property : vecPkProperties)
        teDataSetType->remove(property);
    }

    std::unique_ptr<te::mem::DataSet> ds(new te::mem::DataSet(teDataSetType.get()));

    // Insert data
    for(const auto& rasterInfo : vecRasterInfo)
    {
      auto it = std::find_if(vecDates.begin(), vecDates.end(),
                             [&rasterInfo](std::shared_ptr<te::dt::DateTime> const& first)
                             {
                               return *first == rasterInfo.timeTz;
                             });

      if(it == std::end(vecDates))
      {
        std::unique_ptr<te::gm::Geometry> geom (te::gm::GetGeomFromEnvelope(rasterInfo.envelope.get(), rasterInfo.srid));

        std::string fileURI = baseUrl.toLocalFile().toStdString();
        QFileInfo file(QString::fromStdString(fileURI + "/" + terrama2::core::getFolderMask(dataset) + "/" + rasterInfo.name));
        if(!file.exists())
        {

          QString errMsg = QObject::tr("Unable to locate file: %1").arg(file.absolutePath());
          TERRAMA2_LOG_ERROR() << errMsg;
          throw Exception() << ErrorDescription(errMsg);
        }

        std::unique_ptr<te::mem::DataSetItem> dsItem (new te::mem::DataSetItem(ds.get()));
        dsItem->setGeometry("the_geom", geom.release());
        dsItem->setString("location", file.absoluteFilePath().toStdString());
        dsItem->setDateTime("timestamp", new te::dt::TimeInstant(rasterInfo.timeTz));

        ds->add(dsItem.release());
      }
    }

    /*
     * Resetting properties files tree.
     * Now we defined a sub directory containing both LayerName.properties and datastore.properties.
     */
    recreateFolder(url.path().toStdString());
    // Creating LayerName.properties
    createPostgisMosaicLayerPropertiesFile(url.path().toStdString(), layerName, vecRasterInfo[0]);
    // Create datastore.properties
    createPostgisDatastorePropertiesFile(url.path().toStdString(), connInfo);

    if(!ds->isEmpty())
    {
      ds->moveBeforeFirst();

      std::map<std::string, std::string> options;
      dataSource->add(layerName, ds.get(), options);

      // register datastore and layer if they don't exists
      registerMosaicCoverage(viewPtr, layerName, url.path().toStdString(), layerName, vecRasterInfo[0], "", "all");
    }

    layersNames.push_back(layerName);
  }

  return layersNames;
}

std::vector<terrama2::services::view::core::RasterInfo>
terrama2::services::view::core::GeoServer::getRasterInfo(terrama2::core::DataManagerPtr dataManager,
                                                         terrama2::core::DataSetPtr dataset,
                                                         const terrama2::core::Filter& filter) const
{
  auto dataSeries = dataManager->findDataSeries(dataset->dataSeriesId);

  if(!dataSeries)
  {
    QString errMsg = QObject::tr("Could not find the data series: %1").arg(dataset->dataSeriesId);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw Exception() << ErrorDescription(errMsg);
  }

  auto dataProvider = dataManager->findDataProvider(dataSeries->dataProviderId);

  if(!dataProvider)
  {
    QString errMsg = QObject::tr("Could not find the data provider: %1").arg(dataSeries->dataProviderId);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw Exception() << ErrorDescription(errMsg);
  }

  auto dataAccessor = std::dynamic_pointer_cast<terrama2::core::DataAccessorFile>(terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeries));
  auto remover = std::make_shared<terrama2::core::FileRemover>();
  std::unordered_map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > dataMap = dataAccessor->getSeries(filter, remover);

  std::vector<RasterInfo> vecRasterInfo;

  for(const auto& data : dataMap)
  {
    const auto& dataSetSeries = data.second;

    auto rasterProperty = te::da::GetFirstRasterProperty(dataSetSeries.teDataSetType.get());
    if(!rasterProperty)
    {
      QString errMsg = QObject::tr("Could a valid raster property for dataset: %1").arg(dataset->id);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw Exception() << ErrorDescription(errMsg);
    }

    auto datePropertyPos = te::da::GetFirstPropertyPos(dataSetSeries.syncDataSet->dataset().get(), te::dt::DATETIME_TYPE);

    for(unsigned int row = 0; row < dataSetSeries.syncDataSet->size(); ++row)
    {
      auto date = dataSetSeries.syncDataSet->getDateTime(row, datePropertyPos);
      std::shared_ptr<te::dt::TimeInstantTZ> tiTz(dynamic_cast<te::dt::TimeInstantTZ*>(date->clone()));
      auto boostTiTz = tiTz->getTimeInstantTZ();

      QFileInfo info(QString::fromStdString(dataSetSeries.syncDataSet->getString(row, "filename")));

      auto raster = dataSetSeries.syncDataSet->getRaster(row, rasterProperty->getId());

      auto grid = raster->getGrid();

      std::unique_ptr<te::gm::Envelope> extent(new te::gm::Envelope(*raster->getExtent()));

      RasterInfo rinfo;
      rinfo.name = info.fileName().toStdString();
      rinfo.timeTz = te::dt::TimeInstant(boostTiTz.utc_time()), raster->getSRID();
      rinfo.srid = raster->getSRID();
      rinfo.resolutionX = grid->getResolutionX();
      rinfo.resolutionY = grid->getResolutionY();
      rinfo.envelope = std::move(extent);
      vecRasterInfo.push_back(std::move(rinfo));
    }
  }

  return vecRasterInfo;
}


std::string terrama2::services::view::core::GeoServer::createPostgisDatastorePropertiesFile(const std::string& outputFolder,
                                                                                            const te::core::URI& connInfo) const
{
    std::string propertiesFilename = outputFolder + "/datastore.properties";

    QFile outputFile(propertiesFilename.c_str());

    outputFile.open(QIODevice::WriteOnly);

    /* Check it opened OK */
    if(!outputFile.isOpen())
    {
      QString errMsg = QObject::tr("Could not open file: %1").arg(propertiesFilename.c_str());
      TERRAMA2_LOG_ERROR() << errMsg;
      throw Exception() << ErrorDescription(errMsg);
    }

    std::string database = connInfo.path();
    database.erase(std::remove(database.begin(), database.end(), '/'), database.end());

    std::string content = "SPI=org.geotools.data.postgis.PostgisNGDataStoreFactory\n"
                          "host=" + connInfo.host() + "\n"
                                                      "port=" + connInfo.port() + "\n" +
                          "database=" + database + "\n" +
                          "schema=public\n"
                          "user=" + connInfo.user() + "\n" +
                          "passwd=" + connInfo.password() + "\n" +
                          "Loose\\ bbox=true\n"
                          "Estimated\\ extends=false\n"
                          "validate\\ connections=true\n"
                          "Connection\\ timeout=10\n"
                          "preparedStatements=true\n";

    QTextStream outStream(&outputFile);
    outStream << content.c_str();

    /* Close the file */
    outputFile.close();

    return propertiesFilename;
}

std::string terrama2::services::view::core::GeoServer::createPostgisMosaicLayerPropertiesFile(const std::string& outputFolder,
                                                                                              const std::string& exhibitionName,
                                                                                              const RasterInfo& rasterInfo) const
{
  std::string propertiesFilename = outputFolder + "/" + exhibitionName + ".properties";

  QFile outputFile(propertiesFilename.c_str());

  outputFile.open(QIODevice::WriteOnly);

  /* Check it opened OK */
  if(!outputFile.isOpen())
  {
    QString errMsg = QObject::tr("Could not open file: %1").arg(propertiesFilename.c_str());
    TERRAMA2_LOG_ERROR() << errMsg;
    throw Exception() << ErrorDescription(errMsg);
  }

  std::string content = "MosaicCRS=EPSG\\:" + std::to_string(rasterInfo.srid) + "\n" +
                        "Levels=" + toString(rasterInfo.resolutionX) + "," + toString(rasterInfo.resolutionY) + "\n"
                        "Heterogeneous=false\n"
                        "TimeAttribute=timestamp\n"
                        "AbsolutePath=true\n"
                        "Name=" + exhibitionName + "\n" +
                        "TypeName=" + exhibitionName + "\n" +
                        "Caching=false\n"
                        "ExpandToRGB=false\n"
                        "LocationAttribute=location\n"
                        "SuggestedSPI=it.geosolutions.imageioimpl.plugins.tiff.TIFFImageReaderSpi\n"
                        "CheckAuxiliaryMetadata=false\n"
                        "LevelsNum=1";

  QTextStream outStream(&outputFile);
  outStream << content.c_str();

  /* Close the file */
  outputFile.close();

  return propertiesFilename;
}

void terrama2::services::view::core::GeoServer::createPostgisIndexerPropertiesFile(const std::string& outputFolder,
                                                                                   const std::string& exhibitionName) const
{
  std::string propertiesFilename = outputFolder + "/indexer.properties";

  QFile outputFile(propertiesFilename.c_str());

  outputFile.open(QIODevice::WriteOnly);

  /* Check it opened OK */
  if(!outputFile.isOpen())
  {
    QString errMsg = QObject::tr("Could not open file: %1").arg(propertiesFilename.c_str());
    TERRAMA2_LOG_ERROR() << errMsg;
    throw Exception() << ErrorDescription(errMsg);
  }

  std::string content = "Schema=*the_geom:Polygon,location:String,timestamp:java.util.Date\n"
                        "PropertyCollectors=TimestampFileNameExtractorSPI[timeregex](timestamp)\n"
                        "TimeAttribute=timestamp\n"
                        "Name=" + exhibitionName +"\n" +
                        "TypeName=" + exhibitionName +"\n" +
                        "Caching=false\n";

  QTextStream outStream(&outputFile);
  outStream << content.c_str();

  /* Close the file */
  outputFile.close();
}

void terrama2::services::view::core::GeoServer::createTimeregexPropertiesFile(const std::string& outputFolder,
                                                                              const std::string& regex) const
{
  std::string propertiesFilename = outputFolder + "/timeregex.properties";

  QFile outputFile(propertiesFilename.c_str());

  outputFile.open(QIODevice::WriteOnly);

  /* Check it opened OK */
  if(!outputFile.isOpen())
  {
    QString errMsg = QObject::tr("Could not open file: %1").arg(propertiesFilename.c_str());
    TERRAMA2_LOG_ERROR() << errMsg;
    throw Exception() << ErrorDescription(errMsg);
  }

  std::string content = "regex=" + regex + "\n";

  QTextStream outStream(&outputFile);
  outStream << content.c_str();

  /* Close the file */
  outputFile.close();
}


void terrama2::services::view::core::GeoServer::createGeoserverPropertiesFile(const std::string& outputFolder,
                                                                              const std::string& exhibitionName,
                                                                              DataSeriesId dataSeriesId) const
{
  std::string propertiesFilename = outputFolder + "/data_series_" + std::to_string(dataSeriesId) + ".properties";

  std::string content = "Levels=0.009999999776482582,0.009999999776482582\n"
                        "Heterogeneous=false\n"
                        "TimeAttribute=timestamp\n"
                        "AbsolutePath=false\n"
                        "Name=" + exhibitionName +"\n" +
                        "TypeName=data_series_" + std::to_string(dataSeriesId) +"\n" +
                        "Caching=false\n"
                        "ExpandToRGB=false\n"
                        "LocationAttribute=filename\n"
                        "SuggestedSPI=it.geosolutions.imageioimpl.plugins.tiff.TIFFImageReaderSpi\n"
                        "CheckAuxiliaryMetadata=false\n"
                        "LevelsNum=1";

  /* Try and open a file for output */

  QFile outputFile(propertiesFilename.c_str());
  outputFile.open(QIODevice::WriteOnly);

  /* Check it opened OK */
  if(!outputFile.isOpen())
  {
    QString errMsg = QObject::tr("Could not open file: %1").arg(propertiesFilename.c_str());
    TERRAMA2_LOG_ERROR() << errMsg;
    throw Exception() << ErrorDescription(errMsg);
  }

  QTextStream outStream(&outputFile);
  outStream << content.c_str();

  /* Close the file */
  outputFile.close();
}

int terrama2::services::view::core::GeoServer::createGeoserverTempMosaic(terrama2::core::DataManagerPtr dataManager,
                                                                         terrama2::core::DataSetPtr dataset,
                                                                         const terrama2::core::Filter& filter,
                                                                         const std::string& exhibitionName,
                                                                         const std::string& outputFolder) const
{
  auto dataSeries = dataManager->findDataSeries(dataset->dataSeriesId);

  if(!dataSeries)
  {
    QString errMsg = QObject::tr("Could not find the data series: %1").arg(dataset->dataSeriesId);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw Exception() << ErrorDescription(errMsg);
  }

  auto dataProvider = dataManager->findDataProvider(dataSeries->dataProviderId);

  if(!dataProvider)
  {
    QString errMsg = QObject::tr("Could not find the data provider: %1").arg(dataSeries->dataProviderId);
    TERRAMA2_LOG_ERROR() << errMsg;
    throw Exception() << ErrorDescription(errMsg);
  }

  // Delete file if it exists.
  std::string shpFilename = outputFolder + "/data_series_" + std::to_string(dataSeries->id) + ".shp";
  QFileInfo checkFile(shpFilename.c_str());
  if(checkFile.exists())
  {
    QFile file(shpFilename.c_str());
    file.remove();
  }

  auto dataAccessor = std::dynamic_pointer_cast<terrama2::core::DataAccessorFile>(terrama2::core::DataAccessorFactory::getInstance().make(dataProvider, dataSeries));
  auto remover = std::make_shared<terrama2::core::FileRemover>();
  std::unordered_map<terrama2::core::DataSetPtr, terrama2::core::DataSetSeries > dataMap = dataAccessor->getSeries(filter, remover);

  std::unique_ptr<te::mem::DataSet> ds;
  std::unique_ptr<te::da::DataSetType> dt (new te::da::DataSetType(dataSeries->name));
  int geomSRID = 0;

  for(const auto& data : dataMap)
  {
    const auto& dataSetSeries = data.second;
    auto rasterProperty = te::da::GetFirstRasterProperty(dataSetSeries.teDataSetType.get());
    if(!rasterProperty)
    {
      QString errMsg = QObject::tr("Could a valid raster property for dataset: %1").arg(dataset->id);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw Exception() << ErrorDescription(errMsg);
    }

    auto datePropertyPos = te::da::GetFirstPropertyPos(dataSetSeries.syncDataSet->dataset().get(), te::dt::DATETIME_TYPE);

    for(unsigned int row = 0; row < dataSetSeries.syncDataSet->size(); ++row)
    {
      auto raster = dataSetSeries.syncDataSet->getRaster(row, rasterProperty->getId());

      // If is the first row, configure dataSet
      if(row == 0)
      {
        geomSRID = raster->getSRID();

        std::unique_ptr<te::dt::SimpleProperty> filenameProp (new te::dt::SimpleProperty("filename", te::dt::STRING_TYPE, true));
        std::unique_ptr<te::gm::GeometryProperty> geomProp (new te::gm::GeometryProperty("geom", 0, te::gm::PolygonType, true));
        geomProp->setSRID(geomSRID);

        std::unique_ptr<te::dt::DateTimeProperty> timestampProp(new te::dt::DateTimeProperty("timestamp", te::dt::TIME_INSTANT, true));

        dt->add(filenameProp.release());
        dt->add(geomProp.release());
        dt->add(timestampProp.release());

        ds.reset(new te::mem::DataSet(dt.get()));
      }

      auto date = dataSetSeries.syncDataSet->getDateTime(row, datePropertyPos);
      std::shared_ptr<te::dt::TimeInstantTZ> tiTz(dynamic_cast<te::dt::TimeInstantTZ*>(date->clone()));
      auto boostTiTz = tiTz->getTimeInstantTZ();

      std::unique_ptr<te::gm::Geometry> geom (te::gm::GetGeomFromEnvelope(raster->getExtent(), raster->getSRID()));

      std::unique_ptr<te::mem::DataSetItem> dsItem01 (new te::mem::DataSetItem(ds.get()));
      QFileInfo info(QString::fromStdString(dataSetSeries.syncDataSet->getString(row, "filename")));
      dsItem01->setString(0, info.fileName().toStdString());
      dsItem01->setGeometry(1, geom.release());
      dsItem01->setDateTime(2, new te::dt::TimeInstant(boostTiTz.utc_time()));

      ds->add(dsItem01.release());
    }
  }

  {
    if(!ds)
    {
      QString errMsg = QObject::tr("Error creating te::DataSet for dataset: %1").arg(dataset->id);
      TERRAMA2_LOG_ERROR() << errMsg;
      throw Exception() << ErrorDescription(errMsg);
    }

    std::shared_ptr<te::da::DataSource> dsOGR = te::da::DataSourceFactory::make("OGR", "file://"+shpFilename);
    terrama2::core::OpenClose<std::shared_ptr<te::da::DataSource>> openClose(dsOGR);

    ds->moveBeforeFirst();
    te::da::Create(dsOGR.get(), dt.get(), ds.get());
  }

  createGeoserverPropertiesFile(outputFolder, exhibitionName, dataSeries->id);

  return geomSRID;
}


void terrama2::services::view::core::GeoServer::createMosaicTable(std::shared_ptr< te::da::DataSource > dataSource,
                                                                  const std::string& tableName,
                                                                  int srid) const
{
  std::shared_ptr<te::da::DataSourceTransactor> transactor = dataSource->getTransactor();

  std::shared_ptr< te::da::DataSetType > dt(new te::da::DataSetType(tableName));

  std::unique_ptr<te::gm::GeometryProperty> geomProp(new te::gm::GeometryProperty("the_geom", 0, te::gm::PolygonType, true));
  geomProp->setSRID(srid);

  // the newDataSetType takes ownership of the pointer
  std::unique_ptr<te::da::Index> spatialIndex(new te::da::Index("spatial_index_" + tableName, te::da::B_TREE_TYPE, {geomProp.get()}));

  std::unique_ptr<te::dt::StringProperty> filenameProp(new te::dt::StringProperty("location", te::dt::VAR_STRING, 255, true));

  std::unique_ptr<te::dt::DateTimeProperty> timestampProp(new te::dt::DateTimeProperty("timestamp", te::dt::TIME_INSTANT, true));

  std::shared_ptr< te::dt::SimpleProperty > serialPk(new te::dt::SimpleProperty("fid", te::dt::INT32_TYPE, true));
  serialPk->setAutoNumber(true);

  dt->add(serialPk->clone());
  dt->add(geomProp.release());
  dt->add(spatialIndex.release());
  dt->add(filenameProp.release());
  dt->add(timestampProp.release());

  transactor->createDataSet(dt.get(), {});

  std::shared_ptr<te::dt::Property> id_pk1 = transactor->getProperty(dt->getName(),"fid");
  auto pk = new te::da::PrimaryKey(tableName + "_pkey");
  pk->add(id_pk1.get());

  transactor->addPrimaryKey(dt->getName(),pk);

  transactor->commit();
}

std::string terrama2::services::view::core::GeoServer::workspaceName(const ViewPtr& viewPtr)
{
  try
  {
    return viewPtr->properties.at(ViewTags::WORKSPACE);
  }
  catch (const std::out_of_range&)
  {
    return "terrama2_" + std::to_string(viewPtr->id);
  }
}

std::string terrama2::services::view::core::GeoServer::viewLayerName(const ViewPtr& viewPtr) const
{
  try
  {
    return viewPtr->properties.at(ViewTags::LAYER_NAME);
  }
  catch (const std::out_of_range&)
  {
    return "view" + std::to_string(viewPtr->id);
  }
}

std::string terrama2::services::view::core::GeoServer::viewLayerTitle(const ViewPtr& viewPtr) const
{
  try
  {
    return viewPtr->properties.at(ViewTags::LAYER_TITLE);
  }
  catch (const std::out_of_range&)
  {
    return viewPtr->viewName;
  }
}

std::string terrama2::services::view::core::GeoServer::getAttributeName(const terrama2::services::view::core::View::Legend& legend) const
{
  const auto& metadata = legend.metadata;
  auto it = metadata.find("attribute");
  if(it != metadata.end())
    return it->second;

// retro-compatibility
  it = metadata.find("column");
  if(it != metadata.end())
    return it->second;

  QString errMsg = QObject::tr("No legend attribute defined.");
  TERRAMA2_LOG_ERROR() << errMsg;
  throw Exception() << ErrorDescription(errMsg);
}
