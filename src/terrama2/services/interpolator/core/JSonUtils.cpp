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
  \file interpolator/core/JSonUtils.cpp

  \author Frederico Augusto Bedê Teotônio
*/

#include "JSonUtils.hpp"

#include "../../../core/utility/GeoUtils.hpp"
#include "../../../core/utility/JSonUtils.hpp"
#include "../../../core/utility/Logger.hpp"
#include "InterpolatorParams.hpp"

// TerraLib
#include <terralib/geometry/Point.h>
#include <terralib/geometry/WKTWriter.h>

// Qt
#include <QJsonDocument>
#include <QJsonArray>

/*!
 * \brief Transforms the envelope into a JSon object.
 *
 * \param env Envelope to be used.
 *
 * \return JSon object representing the bounding rect.
 */
QJsonObject GetBoundingRect(const te::gm::Envelope& env)
{
  QJsonObject obj;
  std::unique_ptr<te::gm::Point> p1(new te::gm::Point(env.getLowerLeftX(), env.getLowerLeftY()));
  std::unique_ptr<te::gm::Point> p2(new te::gm::Point(env.getUpperRightX(), env.getUpperRightY()));
  std::ostringstream llOs,
      urOs;

  te::gm::WKTWriter::write(p1.get(), llOs);
  te::gm::WKTWriter::write(p2.get(), urOs);

  obj["ll_corner"] = QString::fromStdString(llOs.str());
  obj["ur_corner"] = QString::fromStdString(urOs.str());

  return obj;
}

terrama2::services::interpolator::core::InterpolatorParams* terrama2::services::interpolator::core::fromInterpolatorJson(QJsonObject json, terrama2::core::DataManager* dataManager)
{
  InterpolatorParams* res = 0;

  if(json["class"].toString() != "Interpolator")
  {
    QString errMsg = QObject::tr("Invalid Interpolator JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  if(!(json.contains("id")
       && json.contains("project_id")
       && json.contains("service_instance_id")
       && json.contains("input_data_serQJsonObjecties")
       && json.contains("output_data_series")
       && json.contains("bounding_rect")
       && json.contains("interpolator_strategy")
       && json.contains("number_of_neighbors")
       && json.contains("resolution_x")
       && json.contains("resolution_y")
       && json.contains("interpolation_attribute")
       && json.contains("srid")
       && json.contains("filter")
       && json.contains("active")))
  {
    QString errMsg = QObject::tr("Invalid Interpolator JSON object.");
    TERRAMA2_LOG_ERROR() << errMsg;
    throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
  }

  InterpolatorParams* params = new InterpolatorParams;
  InterpolatorParamsPtr parPtr(params);

  parPtr->id_ = static_cast<uint32_t>(json["id"].toInt());
  parPtr->projectId_ = static_cast<uint32_t>(json["project_id"].toInt());
  parPtr->serviceInstanceId_ = static_cast<uint32_t>(json["service_instance_id"].toInt());

  parPtr->series_ = static_cast<uint32_t>(json["input_data_series"].toInt());
  parPtr->outSeries_ = static_cast<uint32_t>(json["output_data_series"].toInt());

  parPtr->filter_ = terrama2::core::fromFilterJson(json["filter"].toObject(), dataManager);
  parPtr->active_ = json["active"].toBool();

  parPtr->interpolationType_ = static_cast<terrama2::services::interpolator::core::InterpolatorType>(json["interpolator_strategy"].toInt());
  parPtr->attributeName_ = json["interpolation_attribute"].toString().toStdString();
  parPtr->resolutionX_ = json["resolution_x"].toDouble();
  parPtr->resolutionY_ = json["resolution_y"].toDouble();
  parPtr->srid_ = json["srid"].toInt();
  parPtr->numNeighbors_ = json["number_of_neighbors"].toInt();

  auto bboxObj = json["bounding_rect"].toObject();

  std::string llS = bboxObj.take("ll_corner").toString().toStdString(),
            urS = bboxObj.take("ur_corner").toString().toStdString();

  auto gLl = terrama2::core::ewktToGeom(llS);
  auto gUr = terrama2::core::ewktToGeom(urS);

  auto ll = std::dynamic_pointer_cast<te::gm::Point>(gLl),
      ur = std::dynamic_pointer_cast<te::gm::Point>(gUr);

  te::gm::Envelope env(ll->getX(), ll->getY(), ur->getX(), ur->getY());

  parPtr->bRect_ = env;

  return res;
}

QJsonObject terrama2::services::interpolator::core::toJson(InterpolatorParams* params)
{
  QJsonObject obj;
  obj.insert("class", QString("Interpolator"));
  obj.insert("id", static_cast<int32_t>(params->id_));
  obj.insert("project_id", static_cast<int32_t>(params->projectId_));
  obj.insert("service_instance_id", static_cast<int32_t>(params->serviceInstanceId_));
  obj.insert("input_data_series", static_cast<int32_t>(params->series_));
  obj.insert("output_data_series", static_cast<int32_t>(params->outSeries_));
  obj.insert("interpolator_strategy", static_cast<int32_t>(params->interpolationType_));
  obj.insert("number_of_neighbors", static_cast<int32_t>(params->numNeighbors_));
  obj.insert("resolution_x", static_cast<double>(params->resolutionX_));
  obj.insert("resolution_y", static_cast<double>(params->resolutionY_));
  obj.insert("interpolation_attribute", QString::fromStdString(params->attributeName_));
  obj.insert("srid", static_cast<int32_t>(params->srid_));
  obj.insert("active", params->active_);

  obj.insert("bounding_rect", GetBoundingRect(params->bRect_));

  return obj;
}
