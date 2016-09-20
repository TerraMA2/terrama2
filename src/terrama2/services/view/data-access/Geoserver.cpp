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
  \file terrama2/services/view/data-access/Geoserver.cpp

  \brief Communication class between View service and GeoServer

  \author Vinicius Campanha
*/

// Qt
#include <QTemporaryFile>

// TerraLib
#include <terralib/ws/core/CurlWrapper.h>

// TerraMA2
#include "Geoserver.hpp"
#include "Exception.hpp"
#include "../core/JSonUtils.hpp"
#include "../../../core/utility/Logger.hpp"


terrama2::services::view::data_access::GeoServer::GeoServer(const te::core::URI uri, const std::string workspace)
  : uri_(uri), workspace_(workspace)
{

}


void terrama2::services::view::data_access::GeoServer::setWorkspace(const std::string& workspace)
{
  workspace_ = workspace;
}


void terrama2::services::view::data_access::GeoServer::registerWorkspace(const std::string& name) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  workspace_ = name;

  te::core::URI uriPost(uri_.uri() + "/rest/workspaces");

  if(!uriPost.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriPost.uri();
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg + QString::fromStdString(uriPost.uri()));
  }

  // Register style
  cURLwrapper.post(uriPost, "<workspace><name>" + workspace_ + "</name></workspace>", "Content-Type: text/xml");
}


void terrama2::services::view::data_access::GeoServer::uploadVectorFile(const std::string& dataStoreName, const std::string& shpPath, const std::string& extension) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriPut(uri_.uri() + "/rest/workspaces/" + workspace_ + "/datastores/" + dataStoreName + "/file." + extension);

  if(!uriPut.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriPut.uri();
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg + QString::fromStdString(uriPut.uri()));
  }
  // Upload Vector file
  cURLwrapper.putFile(uriPut, shpPath, "Content-type: application/zip");
}


void terrama2::services::view::data_access::GeoServer::uploadCoverageFile(const std::string& coverageStoreName, const std::string& coverageFilePath, const std::string& extension) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriPut(uri_.uri() + "/rest/workspaces/" + workspace_ + "/coveragestores/" + coverageStoreName + "/file." + extension );

  if(!uriPut.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriPut.uri();
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg + QString::fromStdString(uriPut.uri()));
  }
  // Upload Coverage file
  cURLwrapper.putFile(uriPut, coverageFilePath, "Content-type: application/zip");
}


void terrama2::services::view::data_access::GeoServer::registerStyle(const std::string& name, const std::string& styleFilePath) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriPost(uri_.uri() + "/rest/workspaces/" + workspace_ + "/styles");

  if(!uriPost.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriPost.uri();
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg + QString::fromStdString(uriPost.uri()));
  }

  // Register style
  cURLwrapper.post(uriPost, "<style><name>" + name + "</name><filename>" + name + ".sld</filename></style>", "Content-Type: text/xml");


  te::core::URI uriPut(uri_.uri() + "/rest/workspaces/" + workspace_ + "/styles/" + name);

  if(!uriPut.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriPut.uri();
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg + QString::fromStdString(uriPut.uri()));
  }

  // Upload Style file
  cURLwrapper.putFile(uriPut, styleFilePath, "Content-type: application/vnd.ogc.se+xml");
}


void terrama2::services::view::data_access::GeoServer::registerStyle(const std::string &name, const std::unique_ptr<te::se::Style> &style) const
{
  QTemporaryFile file;
  if(!file.open())
  {
    QString errMsg = QObject::tr("Can't open the file.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  std::string filePath = file.fileName().toStdString();
  // VINICIUS: move all serialization from json utils to another file
  terrama2::services::view::core::writeStyleGeoserverXML(style.get(), filePath);

  QByteArray content = file.readAll();
  if(content.isEmpty())
  {
    QString errMsg = QObject::tr("Can't read the SLD file.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg);
  }

  registerStyle(name, filePath);
}


void terrama2::services::view::data_access::GeoServer::deleteWorkspace(bool recursive) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriDelete(uri_.uri() + "/rest/workspaces/" + workspace_);

  if(!uriDelete.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriDelete.uri();
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg + QString::fromStdString(uriDelete.uri()));
  }

  if(recursive)
  {
    cURLwrapper.customRequest(uriDelete, "delete&recurse=true");
  }
  else
  {
    cURLwrapper.customRequest(uriDelete, "delete&recurse=false");
  }
}


void terrama2::services::view::data_access::GeoServer::deleteVectorFile(const std::string& dataStoreName, const std::string &fileName, bool recursive) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriDelete(uri_.uri() + "/rest/workspaces/" + workspace_ + "/datastores/" + dataStoreName + "/featuretypes/" + fileName);

  if(!uriDelete.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriDelete.uri();
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg + QString::fromStdString(uriDelete.uri()));
  }

  if(recursive)
  {
    cURLwrapper.customRequest(uriDelete, "delete&recurse=true");
  }
  else
  {
    cURLwrapper.customRequest(uriDelete, "delete&recurse=false");
  }
}


void terrama2::services::view::data_access::GeoServer::deleteCoverageFile(const std::string& coverageStoreName, const std::string& fileName, bool recursive) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriDelete(uri_.uri() + "/rest/workspaces/" + workspace_ + "/coveragestores/" + coverageStoreName + "/coverages/" + fileName);

  if(!uriDelete.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriDelete.uri();
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg + QString::fromStdString(uriDelete.uri()));
  }

  if(recursive)
  {
    cURLwrapper.customRequest(uriDelete, "delete&recurse=true");
  }
  else
  {
    cURLwrapper.customRequest(uriDelete, "delete&recurse=false");
  }
}


void terrama2::services::view::data_access::GeoServer::deleteStyle(const::std::string& styleName) const
{
  te::ws::core::CurlWrapper cURLwrapper;

  te::core::URI uriDelete(uri_.uri() + "/rest/styles/" + styleName);

  if(!uriDelete.isValid())
  {
    QString errMsg = QObject::tr("Invalid URI.");
    TERRAMA2_LOG_ERROR() << errMsg << uriDelete.uri();
    throw terrama2::InvalidArgumentException() << ErrorDescription(errMsg + QString::fromStdString(uriDelete.uri()));
  }

    cURLwrapper.customRequest(uriDelete, "delete&purge=true");
}
