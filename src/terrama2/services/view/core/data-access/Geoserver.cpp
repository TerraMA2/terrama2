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
#include "../../core/JSonUtils.hpp"
#include "../../../../impl/DataAccessorFile.hpp"
#include "../../../../impl/DataAccessorPostGIS.hpp"
#include "../../../../core/data-model/DataProvider.hpp"
#include "../../../../core/utility/Raii.hpp"
#include "../../../../core/utility/Logger.hpp"
#include "../../../../core/utility/DataAccessorFactory.hpp"
#include "../../../../core/utility/Utils.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/utils/Utils.h>
#include <terralib/ws/core/CurlWrapper.h>
#include <terralib/ws/ogc/wms/client/WMSClient.h>
#include <terralib/geometry/Envelope.h>
#include <terralib/fe/PropertyName.h>
#include <terralib/fe/Literal.h>
#include <terralib/fe/BinaryComparisonOp.h>
#include <terralib/fe/Filter.h>
#include <terralib/fe/Globals.h>

// Qt
#include <QTemporaryFile>
#include <QUrl>


terrama2::services::view::core::GeoServer::GeoServer(const te::core::URI uri)
  : MapsServer(uri)
{

}


const te::core::URI& terrama2::services::view::core::GeoServer::uri() const
{
  return uri_;
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
                      "<passwd>" + connInfo.at("PG_USER") + "</passwd>" +
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
      QString errMsg = QObject::tr("Error at register PostGis Table. ");
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


void terrama2::services::view::core::GeoServer::registerPostgisTable(const std::string& dataStoreName,
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
  xml += "<title>" + layerName + "</title>";
  xml += "<name>" + layerName + "</name>";
  xml += "<nativeName>" + tableName + "</nativeName>";
  xml += "<enabled>true</enabled>";

  std::string metadataTime = "";
  std::string metadataSQL = "";

  if(!timestampPropertyName.empty())
  {
    metadataTime = "<entry key=\"time\">"
                   "<dimensionInfo>"
                   "<enabled>true</enabled>"
                   "<attribute>"+timestampPropertyName+"</attribute>"+
                   "<presentation>CONTINUOUS_INTERVAL</presentation>"
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

    metadataSQL = "<entry key=\"JDBC_VIRTUAL_TABLE\">"
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


void terrama2::services::view::core::GeoServer::registerVectorFile(const std::string& dataStoreName,
                                                                   const std::string& shpFilePath,
                                                                   const std::string& layerName) const
{
  std::string store = QString(QUrl::toPercentEncoding(QString::fromStdString(dataStoreName), "", "-._~/")).toStdString();

  try
  {
    deleteVectorLayer(store, layerName, true);
  }
  catch(NotFoundGeoserverException /*e*/)
  {
    // Do nothing
  }

  registerVectorDataStore(store, shpFilePath);

  te::ws::core::CurlWrapper cURLwrapper;

  std::string uri = uri_.uri() + "/rest/workspaces/" + workspace_ + "/datastores/"
                    + store +"/featuretypes";

  te::core::URI uriPost(uri);

  if(!uriPost.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriPost.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPost.uri()));
  }

  std::string xmlFeature = "<featureType>";
  xmlFeature += "<title>" + layerName + "</title>";
  xmlFeature += "<name>" + layerName + "</name>";

  QFileInfo file(QString::fromStdString(shpFilePath));
  xmlFeature += "<nativeName>" + file.baseName().toStdString() + "</nativeName>";
  xmlFeature += "<enabled>true</enabled>";
  xmlFeature += "</featureType>";

  cURLwrapper.post(uriPost, xmlFeature, "Content-Type: text/xml");

  if(cURLwrapper.responseCode() != 201)
  {
    QString errMsg = QObject::tr("Error at register PostGis Table. ");
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


void terrama2::services::view::core::GeoServer::registerMosaicCoverage(const std::string& coverageStoreName,
                                                                       const std::string& mosaicPath,
                                                                       const std::string& coverageName,
                                                                       const int srid,
                                                                       const std::string& style) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriPut(uri_.uri() + "/rest/workspaces/" + workspace_ + "/coveragestores/"
                       + QString(QUrl::toPercentEncoding(QString::fromStdString(coverageStoreName), "", "-._~/")).toStdString()
                       + "/external.imagemosaic?configure=all");

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
                                     + QString(QUrl::toPercentEncoding(QString::fromStdString(coverageStoreName), "", "-._~/")).toStdString()
                                     + "/coverages/" + coverageName);

  std::string xml = "<coverage>"
                    "<enabled>true</enabled>"
                    "<srs>EPSG:"
                    + std::to_string(srid) +
                    "</srs>"
                    "<metadata>"
                    "<entry key=\"time\">"
                    "<dimensionInfo>"
                    "<enabled>true</enabled>"
                    "<presentation>CONTINUOUS_INTERVAL</presentation>"
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
                                                              const std::unique_ptr<te::da::DataSetType>& dataSetType) const
{
  if(dataSetType->hasGeom())
  {
    std::unique_ptr<te::se::Style> style(generateVectorialStyle(legend, dataSetType).release());

    registerStyle(name, style);
  }
  else if(dataSetType->hasRaster())
  {
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
                                                                                                 const std::unique_ptr<te::da::DataSetType>& dataSetType) const
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
    std::vector<te::se::Rule*> rules;
    te::se::Rule* ruleDefault;

    for(auto& legendRule : legendRules)
    {
      te::se::Symbolizer* symbolizer(getSymbolizer(dataSetType, legendRule.color));

      te::se::Rule* rule = new te::se::Rule;
      rule->push_back(symbolizer);
      rule->setName(new std::string(legendRule.title));

      if(legendRule.isDefault)
      {
        ruleDefault = rule;
        continue;
      }

      te::fe::PropertyName* propertyName = new te::fe::PropertyName(legend.metadata.at("column"));
      te::fe::Literal* value = new te::fe::Literal(legendRule.value);

      te::fe::BinaryComparisonOp* operation = nullptr;

      if(legend.classify == View::Legend::ClassifyType::VALUES)
      {
        operation = new te::fe::BinaryComparisonOp(te::fe::Globals::sm_propertyIsEqualTo, propertyName, value);
      }
      else if(legend.classify == View::Legend::ClassifyType::INTERVALS)
      {
        operation = new te::fe::BinaryComparisonOp(te::fe::Globals::sm_propertyIsGreaterThanOrEqualTo, propertyName,value);
      }

      if(operation == nullptr)
      {
        QString errMsg = QObject::tr("Unknow classification type!");
        TERRAMA2_LOG_ERROR() << errMsg;
        throw ViewGeoserverException() << ErrorDescription(errMsg);
      }

      te::fe::Filter* filter = new te::fe::Filter;
      filter->setOp(operation);

      rule->setFilter(filter);

      rules.push_back(rule);
    }

    style->push_back(ruleDefault);

    for(auto& rule : rules)
    {
      style->push_back(rule);
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


void terrama2::services::view::core::GeoServer::deleteWorkspace(bool recursive) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  std::string url = "/rest/workspaces/" + workspace_;

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
    QString errMsg = QObject::tr("Error at delete Workspace. ");
    TERRAMA2_LOG_ERROR() << errMsg << uriDelete.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(cURLwrapper.response()));
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

QJsonObject terrama2::services::view::core::GeoServer::generateLayers(const ViewPtr viewPtr,
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

  setWorkspace("terrama2_" + std::to_string(viewPtr->id));

  registerWorkspace();

  QJsonArray layersArray;

  // DataSetType model to use in style creation
  std::unique_ptr< te::da::DataSetType > modelDataSetType;

  if(dataProviderType == "FILE")
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
        std::string layerName = fileInfo.fileName().toStdString();

        if(dataFormat == "OGR")
        {
          if(!modelDataSetType)
          {
            modelDataSetType.reset(DataAccess::getVectorialDataSetType(fileInfo));
          }

          registerVectorFile(layerName + "_datastore_" + std::to_string(viewPtr->id),
                             fileInfo.absoluteFilePath().toStdString(),
                             layerName);
        }
        else if(dataFormat == "GDAL")
        {
          if(!modelDataSetType)
          {
            modelDataSetType.reset(DataAccess::getGeotiffDataSetType(fileInfo));
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

    for(auto& dataset : inputDataSeries->datasetList)
    {
      TableInfo tableInfo = DataAccess::getPostgisTableInfo(dataSeriesProvider, dataset);

      std::string tableName = tableInfo.tableName;
      std::string layerName = terrama2::core::simplifyString(viewPtr->viewName + "_" + std::to_string(viewPtr->id));
      std::string timestampPropertyName = tableInfo.timestampPropertyName;

      modelDataSetType = std::move(tableInfo.dataSetType);

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
          logger->log(ViewLogger::ERROR_MESSAGE, "Data to join is in a different DB.", logId);
          TERRAMA2_LOG_ERROR() << QObject::tr("Cannot join data from a different DB source!");
          continue;
        }

        if(monitoredObjectDataSeries->datasetList.empty())
        {
          logger->log(ViewLogger::ERROR_MESSAGE, "No join data.", logId);
          TERRAMA2_LOG_ERROR() << QObject::tr("Cannot join data from a different DB source!");
          continue;
        }

        const terrama2::core::DataSetPtr monitoredObjectDataset = monitoredObjectDataSeries->datasetList.at(0);

        TableInfo monitoredObjectTableInfo = DataAccess::getPostgisTableInfo(std::make_pair(monitoredObjectDataSeries, monitoredObjectProvider),
                                                                             monitoredObjectDataset);

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

      registerPostgisTable(std::to_string(viewPtr->id) + "_" + std::to_string(inputDataSeries->id) + "_datastore",
                           connInfo,
                           tableName,
                           layerName,
                           modelDataSetType,
                           timestampPropertyName,
                           SQL);

      QJsonObject layer;
      layer.insert("layer", QString::fromStdString(layerName));
      layersArray.push_back(layer);
    }
  }

  if(viewPtr->legend)
  {
    // Register style
    std::string styleName = "";

    styleName = inputDataSeries->name + "_style_" + viewPtr->viewName;
    registerStyle(styleName, *viewPtr->legend.get(), modelDataSetType);

    for(const auto& layer : layersArray)
    {
      registerLayerDefaultStyle(styleName, layer.toObject().value("layer").toString().toStdString());
    }
  }

  jsonAnswer.insert("workspace", QString::fromStdString(workspace()));
  jsonAnswer.insert("layers_list", layersArray);

  return jsonAnswer;
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
      QString errMsg = QObject::tr("Error at register PostGis Table, unknow geometry type. ");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw ViewGeoserverException() << ErrorDescription(errMsg);
      break;
  }
}

