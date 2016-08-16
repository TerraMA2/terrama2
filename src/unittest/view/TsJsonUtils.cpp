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
  \file unittest/view/TsJsonUtils.cpp

  \brief Tests for View JsonUtils class

  \author Vinicius Campanha
*/

#include <terralib/se.h>

//TerraMA2
#include "TsJsonUtils.hpp"

#include <terrama2/services/view/core/JSonUtils.hpp>
#include <terrama2/core/utility/TimeUtils.hpp>


te::se::Stroke* CreateStroke(te::se::Graphic* graphicFill,
                             const std::string& width, const std::string& opacity,
                             const std::string& dasharray, const std::string& linecap, const std::string& linejoin)
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

te::se::Stroke* CreateStroke(const std::string& color, const std::string& width,
                             const std::string& opacity, const std::string& dasharray,
                             const std::string& linecap, const std::string& linejoin)
{
  te::se::Stroke* stroke = CreateStroke(0, width, opacity, dasharray, linecap, linejoin);

  if(!color.empty())
    stroke->setColor(color);

  return stroke;
}

te::se::Fill* CreateFill(const std::string& color, const std::string& opacity)
{
  te::se::Fill* fill = new te::se::Fill;

  if(!color.empty())
    fill->setColor(color);

  if(!opacity.empty())
    fill->setOpacity(opacity);

  return fill;
}

te::se::Symbolizer* getSymbolizer(const te::gm::GeomType& geomType)
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
      te::se::Fill* fill = CreateFill("#5e5eeb", "100");
      te::se::Stroke* stroke = CreateStroke("#000000", "1", "", "", "", "");
      te::se::PolygonSymbolizer* symbolizer = new te::se::PolygonSymbolizer;
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
      te::se::Stroke* stroke = CreateStroke("#000000", "1", "", "", "", "");
      te::se::LineSymbolizer* symbolizer = new te::se::LineSymbolizer;
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
      te::se::Fill* markFill = CreateFill("#5e5eeb", "1.0");
      te::se::Stroke* markStroke = CreateStroke("#000000", "1", "", "", "", "");
      te::se::Mark* mark = CreateMark("circle", markStroke, markFill);
      te::se::Graphic* graphic = CreateGraphic(mark, "12", "", "");
      te::se::PointSymbolizer* symbolizer = new te::se::PointSymbolizer;
      symbolizer->setGraphic(graphic);

      return symbolizer;
    }

    default:
      return nullptr;
  }
}

te::se::Style* CreateFeatureTypeStyle(const te::gm::GeomType& geomType)
{
  te::se::Symbolizer* symbolizer = getSymbolizer(geomType);

  te::se::Rule* rule = new te::se::Rule;

  if(symbolizer != 0)
    rule->push_back(symbolizer);

  te::se::FeatureTypeStyle* style = new te::se::FeatureTypeStyle;
  style->push_back(rule);

  style->setName(new std::string("Style 1"));
//  style->setDescription("description");
  style->setFeatureTypeName(new std::string("Featuretypemname"));
  std::string version = "1";
  style->setVersion(version);

  return style;
}

void TsJsonUtils::testGoNBackJSon()
{
  try
  {
    terrama2::services::view::core::View* view = new terrama2::services::view::core::View();
    terrama2::services::view::core::ViewPtr viewPtr(view);

    view->id = 1;
    view->projectId = 1;
    view->serviceInstanceId = 1;
    view->active = true;
    view->resolutionWidth = 800;
    view->resolutionHeight = 600;
    view->srid = 4326;

    terrama2::core::Schedule schedule;
    schedule.id = 1;
    schedule.frequency = 2;
    schedule.frequencyUnit = "min";

    view->schedule = schedule;

    terrama2::core::Filter filter;
    filter.discardBefore = terrama2::core::TimeUtils::stringToTimestamp("2016-07-06 12:39:00UTM+00", "%Y-%m-%d %H:%M:%S%ZP");
    filter.discardAfter = terrama2::core::TimeUtils::stringToTimestamp("2016-07-06 12:45:00UTM+00", "%Y-%m-%d %H:%M:%S%ZP");

    view->filtersPerDataSeries.emplace(1, filter);
    view->filtersPerDataSeries.emplace(2, filter);
    view->filtersPerDataSeries.emplace(3, filter);
    view->filtersPerDataSeries.emplace(4, filter);

    view->dataSeriesList.push_back(1);
    view->dataSeriesList.push_back(2);
    view->dataSeriesList.push_back(3);
    view->dataSeriesList.push_back(4);

    view->stylesPerDataSeries.emplace(2, std::unique_ptr<te::se::Style>(CreateFeatureTypeStyle(te::gm::PolygonType)));

    QJsonObject obj = terrama2::services::view::core::toJson(viewPtr);

    terrama2::services::view::core::ViewPtr viewBackPtr = terrama2::services::view::core::fromViewJson(obj);

    QCOMPARE(viewBackPtr->id, viewPtr->id);
    QCOMPARE(viewBackPtr->projectId, viewPtr->projectId);
    QCOMPARE(viewBackPtr->serviceInstanceId, viewPtr->serviceInstanceId);
    QCOMPARE(viewBackPtr->active, viewPtr->active);
    QCOMPARE(viewBackPtr->resolutionWidth, viewPtr->resolutionWidth);
    QCOMPARE(viewBackPtr->resolutionHeight, viewPtr->resolutionHeight);

    QCOMPARE(viewBackPtr->schedule.id, viewPtr->schedule.id);
    QCOMPARE(viewBackPtr->schedule.frequency, viewPtr->schedule.frequency);
    QCOMPARE(viewBackPtr->schedule.frequencyUnit, viewPtr->schedule.frequencyUnit);

    QCOMPARE(viewBackPtr->dataSeriesList.size(), viewPtr->dataSeriesList.size());

    for(uint32_t i = 0; i < viewPtr->dataSeriesList.size(); i++)
    {
      QCOMPARE(viewBackPtr->dataSeriesList.at(i), viewPtr->dataSeriesList.at(i));
    }

    QCOMPARE(viewBackPtr->filtersPerDataSeries.size(), viewPtr->filtersPerDataSeries.size());

    // TODO: enable when convert filter/json is implemented
/*    for(auto& it : viewPtr->filtersPerDataSeries)
    {
      QCOMPARE(*viewBackPtr->filtersPerDataSeries.at(it.first).discardBefore, *it.second.discardBefore);
      QCOMPARE(*viewBackPtr->filtersPerDataSeries.at(it.first).discardAfter, *it.second.discardAfter);
    }
*/
    QCOMPARE(viewBackPtr->stylesPerDataSeries.size(), viewPtr->stylesPerDataSeries.size());

    for(auto& it : viewPtr->stylesPerDataSeries)
    {
      QCOMPARE(*viewBackPtr->stylesPerDataSeries.at(it.first)->getName(), *it.second->getName());
      QCOMPARE(viewBackPtr->stylesPerDataSeries.at(it.first)->getVersion(), it.second->getVersion());
    }


  }
  catch(terrama2::Exception& e)
  {
    QString message(*boost::get_error_info<terrama2::ErrorDescription>(e));
    QFAIL(message.toStdString().c_str());
  }
  catch(boost::exception& e)
  {
    QFAIL(boost::diagnostic_information(e).c_str());
  }
  catch(std::exception& e)
  {
    QFAIL(e.what());
  }
  catch(...)
  {
    QFAIL("Unknow exception!");
  }
}
