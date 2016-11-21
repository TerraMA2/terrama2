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
#include "../serialization/Serialization.hpp"
#include "../../core/JSonUtils.hpp"
#include "../../../../core/utility/Logger.hpp"

// TerraLib
#include <terralib/ws/core/CurlWrapper.h>
#include <terralib/ws/ogc/wms/client/WMSClient.h>
#include <terralib/geometry/Envelope.h>

// Qt
#include <QTemporaryFile>
#include <QUrl>


terrama2::services::view::core::GeoServer::GeoServer(const te::core::URI uri)
  : uri_(uri)
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
    QString errMsg = QObject::tr("Workspace not found. ");
    TERRAMA2_LOG_ERROR() << errMsg << cURLwrapper.response();
    throw NotFoundGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(cURLwrapper.response()));
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
                       + QString(QUrl::toPercentEncoding(QString::fromStdString(name), "", "/")).toStdString());

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
    QString errMsg = QObject::tr("Data Store not found. ");
    TERRAMA2_LOG_ERROR() << errMsg << cURLwrapper.response();
    throw NotFoundGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(cURLwrapper.response()));
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
                                                                         std::map<std::string, std::string> connInfo) const
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


const std::string& terrama2::services::view::core::GeoServer::getFeature(const std::string& dataStoreName,
                                                                         const std::string& name) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriGet(uri_.uri() + "/rest/workspaces/" + workspace_ + "/datastores/"
                       +  QString(QUrl::toPercentEncoding(QString::fromStdString(dataStoreName), "", "/")).toStdString()
                       + "/featuretypes/"
                       + QString(QUrl::toPercentEncoding(QString::fromStdString(name), "", "/")).toStdString());

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
    QString errMsg = QObject::tr("Feature not found. ");
    TERRAMA2_LOG_ERROR() << errMsg << cURLwrapper.response();
    throw NotFoundGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(cURLwrapper.response()));
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
                                                                     const std::string& title,
                                                                     const std::string& timestampPropertyName,
                                                                     const std::string& sql) const
{
  try
  {
    getFeature(dataStoreName, tableName);

    deleteVectorLayer(dataStoreName, tableName, true);
  }
  catch(NotFoundGeoserverException /*e*/)
  {
    // Do nothing
  }

  registerPostGisDataStore(dataStoreName, connInfo);

  te::ws::core::CurlWrapper cURLwrapper;

  std::string xml = "<featureType>"
                    "<title>" + title + "</title>";

  xml += "<name>"+ tableName + "</name>";

  xml += "<enabled>true</enabled>";

  std::string metadataTime = "";
  std::string metadataSQL = "";

  if(!timestampPropertyName.empty())
  {
    metadataTime = "<entry key=\"time\">"
                      "<dimensionInfo>"
                        "<enabled>true</enabled>"
                        "<attribute>"+timestampPropertyName+"</attribute>"
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
    metadataSQL = "<entry key=\"JDBC_VIRTUAL_TABLE\">"
                    "<virtualTable>"
                      "<name>"+title+"</name>"
                      "<sql>"+sql+"</sql>"
                      "<escapeSql>false</escapeSql>"
                    "</virtualTable>"
                  "</entry>";
  }

  if(!metadataTime.empty() || !metadataSQL.empty())
  {
    xml += "<metadata>" + metadataTime + metadataSQL + "</metadata>";
  }

  xml += "</featureType>";

  std::string uri = uri_.uri() + "/rest/workspaces/" + workspace_ + "/datastores/"
                    + QString(QUrl::toPercentEncoding(QString::fromStdString(dataStoreName), "", "/")).toStdString()
                    +"/featuretypes";

  te::core::URI uriPostLayer(uri);
  cURLwrapper.post(uriPostLayer, xml, "Content-Type: text/xml");
}


void terrama2::services::view::core::GeoServer::uploadZipVectorFiles(const std::string& dataStoreName,
                                                                   const std::string& shpZipFilePath,
                                                                   const std::string& extension) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriPut(uri_.uri() + "/rest/workspaces/" + workspace_ + "/datastores/"
                       + QString(QUrl::toPercentEncoding(QString::fromStdString(dataStoreName), "", "/")).toStdString()
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
                                                                 const std::string& extension) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriPut(uri_.uri() + "/rest/workspaces/" + workspace_ + "/datastores/"
                       + QString(QUrl::toPercentEncoding(QString::fromStdString(dataStoreName), "", "/")).toStdString()
                       + "/external." + extension + "?configure=first&update=append");

  if(!uriPut.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriPut.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPut.uri()));
  }

  // Register Vector file
  cURLwrapper.customRequest(uriPut, "PUT", "file://" + shpFilePath);
}


void terrama2::services::view::core::GeoServer::registerVectorsFolder(const std::string &dataStoreName,
                                                                    const std::string &shpFolderPath,
                                                                    const std::string &extension) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriPut(uri_.uri() + "/rest/workspaces/" + workspace_ + "/datastores/"
                       + QString(QUrl::toPercentEncoding(QString::fromStdString(dataStoreName), "", "/")).toStdString()
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
                       + QString(QUrl::toPercentEncoding(QString::fromStdString(coverageStoreName), "", "/")).toStdString()
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
                                                                     const std::string& extension,
                                                                     const std::string& style) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriPut(uri_.uri() + "/rest/workspaces/" + workspace_ + "/coveragestores/"
                       + QString(QUrl::toPercentEncoding(QString::fromStdString(coverageStoreName), "", "/")).toStdString()
                       + "/external." + extension + "?configure=first");

  if(!uriPut.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriPut.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriPut.uri()));
  }
  // Upload Coverage file
  cURLwrapper.customRequest(uriPut, "PUT", "file://" + coverageFilePath);

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
  // VINICIUS: move all serialization from json utils to another file
  Serialization::writeStyleGeoserverXML(style.get(), filePath);

  QByteArray content = file.readAll();
  if(content.isEmpty())
  {
    QString errMsg = QObject::tr("Can't read the SLD file.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw ViewGeoserverException() << ErrorDescription(errMsg);
  }

  registerStyleFile(name, filePath);
}


void terrama2::services::view::core::GeoServer::registerStyle(const std::string& name, const std::string &style) const
{
  QTemporaryFile file;

  if(!file.open())
  {
    QString errMsg = QObject::tr("Could not create the XML file!");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw Exception() << ErrorDescription(errMsg);
  }

  file.write(style.c_str());
  file.flush();

  // Upload Style file
  registerStyleFile(name, file.fileName().toStdString());
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

  cURLwrapper.customRequest(uriDelete, "delete");
}


void terrama2::services::view::core::GeoServer::deleteVectorLayer(const std::string& dataStoreName,
                                                               const std::string &fileName,
                                                               bool recursive) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  std::string url = "/rest/workspaces/" + workspace_ + "/datastores/"
                    + QString(QUrl::toPercentEncoding(QString::fromStdString(dataStoreName), "", "/")).toStdString()
                    + "/featuretypes/"
                    + QString(QUrl::toPercentEncoding(QString::fromStdString(fileName), "", "/")).toStdString();

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

  cURLwrapper.customRequest(uriDelete, "delete");
}


void terrama2::services::view::core::GeoServer::deleteCoverageLayer(const std::string& coverageStoreName,
                                                                 const std::string& fileName,
                                                                 bool recursive) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  std::string url = "/rest/workspaces/" + workspace_ + "/coveragestores/"
                    + QString(QUrl::toPercentEncoding(QString::fromStdString(coverageStoreName), "", "/")).toStdString()
                    + "/coverages/"
                    + QString(QUrl::toPercentEncoding(QString::fromStdString(fileName), "", "/")).toStdString();

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

  cURLwrapper.customRequest(uriDelete, "delete");
}


void terrama2::services::view::core::GeoServer::deleteStyle(const std::string& styleName) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriDelete(uri_.uri() + "/rest/workspaces/" + workspace_ + "/styles/"
                          + QString(QUrl::toPercentEncoding(QString::fromStdString(styleName), "", "/")).toStdString());

  if(!uriDelete.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriDelete.uri();
    throw ViewGeoserverException() << ErrorDescription(errMsg + QString::fromStdString(uriDelete.uri()));
  }

  cURLwrapper.customRequest(uriDelete, "delete");
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
