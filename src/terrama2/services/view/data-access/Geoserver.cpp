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
#include "../core/JSonUtils.hpp"


terrama2::services::view::data_access::GeoServer::GeoServer(te::core::URI uri)
  : uri_(uri)
{

}


void terrama2::services::view::data_access::GeoServer::registerStyle(const std::string& name, const std::string styleFilePath)
{
  te::ws::core::CurlWrapper cURLwrapper;

  // Register style
  cURLwrapper.post(uri_, "<style><name>" + name + "</name><filename>" + name + ".sld</filename></style>", "Content-Type: text/xml");


  te::core::URI uriPut(uri_.uri() + "/" + name);

  // Upload Style file
  cURLwrapper.putFile(uriPut, styleFilePath, "Content-type: application/vnd.ogc.se+xml");
}


void terrama2::services::view::data_access::GeoServer::registerStyle(const std::string &name, const std::unique_ptr<te::se::Style> &style)
{
  te::ws::core::CurlWrapper cURLwrapper;

  // Register style
  cURLwrapper.post(uri_, "<style><name>" + name + "</name><filename>" + name + ".sld</filename></style>", "Content-Type: text/xml");

  te::core::URI uriPut(uri_.uri() + "/" + name);

  QTemporaryFile file;
  if(!file.open())
  {
//    std::cout << std::endl << "Could not create XML file!" << std::endl;
  }

  // VINICIUS: move all serialization from json utils to another file
  terrama2::services::view::core::writeStyleGeoserverXML(style.get(), file.fileName().toStdString());

  QByteArray content = file.readAll();
  if(content.isEmpty())
  {
//    std::cout << std::endl << "Could not create XML file!" << std::endl;
  }

  // Upload Style file
  cURLwrapper.put(uriPut, content.data(), "Content-type: application/vnd.ogc.se+xml");
}
