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
  \file terrama2/services/view/core/View.hpp

  \brief Utility functions for view module.

  \author Paulo R. M. Oliveira
*/

// TerraMA2
#include "Utils.hpp"
#include "Exception.hpp"
#include "MapsServerFactory.hpp"
#include "data-access/Geoserver.hpp"
#include "../../../core/data-access/DataAccessor.hpp"
#include "../../../core/data-model/DataManager.hpp"
#include "../../../core/data-model/DataProvider.hpp"
#include "../../../core/data-model/DataSet.hpp"
#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/utility/DataAccessorFactory.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/utility/FileRemover.hpp"
#include "../../../core/utility/TimeUtils.hpp"
#include "../../../core/utility/Utils.hpp"
#include "../../../core/utility/Raii.hpp"

// TerraLib Datasource
#include <terralib/dataaccess/datasource/DataSource.h>
#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

// TerraLib
#include <terralib/se/Categorize.h>
#include <terralib/se/RasterSymbolizer.h>
#include <terralib/se/PolygonSymbolizer.h>
#include <terralib/se/LineSymbolizer.h>
#include <terralib/se/PointSymbolizer.h>
#include <terralib/se/Stroke.h>
#include <terralib/se/Fill.h>
#include <terralib/se/Utils.h>

// STL
#include <sstream>
#include <iomanip>

// QT
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QUrl>

void terrama2::services::view::core::registerFactories()
{
  MapsServerFactory::getInstance().add(terrama2::services::view::core::GeoServer::mapsServerType(),
                                       terrama2::services::view::core::GeoServer::make);
}

te::se::Symbolizer* terrama2::services::view::core::getSymbolizer(const te::gm::GeomType& geomType,
                                                                  const std::string& color,
                                                                  const std::string& opacity) noexcept
{
    switch(geomType)
    {
      case te::gm::PolygonType:
      case te::gm::PolygonMType:
      case te::gm::PolygonZType:
      case te::gm::PolygonZMType:
      case te::gm::MultiPolygonType:
      case te::gm::MultiPolygonMType:
      case te::gm::MultiPolygonZType:
      case te::gm::MultiPolygonZMType:
      case te::gm::MultiSurfaceType:
      case te::gm::MultiSurfaceMType:
      case te::gm::MultiSurfaceZType:
      case te::gm::MultiSurfaceZMType:
      {
        te::se::Fill* fill = CreateFill(color, opacity);
        te::se::Stroke* stroke = CreateStroke("#000000", "1", opacity, "1 0", "", "");

        te::se::PolygonSymbolizer* symbolizer(new te::se::PolygonSymbolizer);

        symbolizer->setFill(fill);
        symbolizer->setStroke(stroke);

        return symbolizer;
      }

      case te::gm::LineStringType:
      case te::gm::LineStringMType:
      case te::gm::LineStringZType:
      case te::gm::LineStringZMType:
      case te::gm::MultiLineStringType:
      case te::gm::MultiLineStringMType:
      case te::gm::MultiLineStringZType:
      case te::gm::MultiLineStringZMType:
      {
        te::se::Stroke* stroke = CreateStroke(color, "1", opacity, "1 0", "", "");

        te::se::LineSymbolizer* symbolizer(new te::se::LineSymbolizer);

        symbolizer->setStroke(stroke);

        return symbolizer;
      }

      case te::gm::PointType:
      case te::gm::PointMType:
      case te::gm::PointZType:
      case te::gm::PointZMType:
      case te::gm::MultiPointType:
      case te::gm::MultiPointMType:
      case te::gm::MultiPointZType:
      case te::gm::MultiPointZMType:
      {
        te::se::Fill* markFill = CreateFill(color, opacity);
        te::se::Stroke* markStroke = CreateStroke("#000000", "1", opacity, "1 0", "", "");
        te::se::Mark* mark = te::se::CreateMark("circle", markStroke, markFill);

        te::se::Graphic* graphic = te::se::CreateGraphic(mark, "8", "", opacity);;

        te::se::PointSymbolizer* symbolizer(new te::se::PointSymbolizer);

        symbolizer->setGraphic(graphic);

        return symbolizer;
      }
      default:
        return nullptr;
    }

  return nullptr;
}


te::se::Stroke* buildStroke(te::se::Graphic* graphicFill,
                            const std::string& width,
                            const std::string& opacity,
                            const std::string& dasharray,
                            const std::string& linecap,
                            const std::string& linejoin)
{
  te::se::Stroke* stroke = new te::se::Stroke;

  if(graphicFill)
    stroke->setGraphicFill(graphicFill);

  if(!width.empty())
    stroke->setWidth(width);

  if(!opacity.empty())
    stroke->setOpacity(opacity);

  if(!dasharray.empty())
    stroke->setDashArray(dasharray);

  if(!linecap.empty())
    stroke->setLineCap(linecap);

  if(!linejoin.empty())
    stroke->setLineJoin(linecap);

  return stroke;
}

te::se::Stroke* terrama2::services::view::core::CreateStroke(const std::string& color,
                                                             const std::string& width,
                                                             const std::string& opacity,
                                                             const std::string& dasharray,
                                                             const std::string& linecap,
                                                             const std::string& linejoin)
{
  te::se::Stroke* stroke = buildStroke(nullptr, width, opacity, dasharray, linecap, linejoin);

  if(!color.empty())
    stroke->setColor(color);

  return stroke;
}

te::se::Fill* terrama2::services::view::core::CreateFill(const std::string& color, const std::string& opacity)
{
  te::se::Fill* fill = new te::se::Fill;

  if(!color.empty())
    fill->setColor(color);

  if(!opacity.empty())
    fill->setOpacity(opacity);

  return fill;
}

std::string terrama2::services::view::core::toString(const double value, const int& precision)
{
  std::stringstream ss;
  ss << std::fixed << std::setprecision(precision) << value;

  return ss.str();
}

void terrama2::services::view::core::removeTable(const std::string& name, const te::core::URI& uri)
{
  std::shared_ptr<te::da::DataSource> dataSource = te::da::DataSourceFactory::make("POSTGIS", uri);
  terrama2::core::OpenClose<std::shared_ptr<te::da::DataSource>> openClose(dataSource);

  if (dataSource->isOpened())
    dataSource->dropDataSet(name);
  else
  {
    const QString errMsg = QObject::tr("Unable to remove view table %1.").arg(QString::fromStdString(name));
    TERRAMA2_LOG_ERROR() << errMsg;
    throw Exception() << ErrorDescription(errMsg);
  }
}

void terrama2::services::view::core::createFolder(const std::string& folderpath)
{
  QDir directory(folderpath.c_str());

  if (!directory.exists())
    if (!directory.mkdir(directory.path()))
    {
      const QString errMsg = QObject::tr("Could not create directory %1").arg(directory.path());
      TERRAMA2_LOG_ERROR() << errMsg;
      throw Exception() << ErrorDescription(errMsg);
    }
}

void terrama2::services::view::core::removeFolder(const std::string& folderpath)
{
  QDir directory(folderpath.c_str());

  if (directory.exists())
    if (!directory.removeRecursively())
    {
      const QString errMsg = QObject::tr("Could not remove directory %1").arg(directory.path());
      TERRAMA2_LOG_ERROR() << errMsg;
      throw Exception() << ErrorDescription(errMsg);
    }
}

void terrama2::services::view::core::recreateFolder(const std::string& folderpath)
{
  removeFolder(folderpath);
  createFolder(folderpath);
}

void terrama2::services::view::core::removeFile(const std::string& filepath)
{
  QFileInfo file(filepath.c_str());

  if (file.exists())
  {
    if (file.isFile())
    {
      QFile f(file.filePath());
      if (!f.remove())
      {
        QString errMsg = QObject::tr("Could not remove file: %1").arg(filepath.c_str());
        TERRAMA2_LOG_ERROR() << errMsg;
        throw Exception() << ErrorDescription(errMsg);
      }
    }
    else
    {
      const QString errMsg = QObject::tr("Not a file: %1").arg(filepath.c_str());
      TERRAMA2_LOG_ERROR() << errMsg;
      throw Exception() << ErrorDescription(errMsg);
    }
  }
}
