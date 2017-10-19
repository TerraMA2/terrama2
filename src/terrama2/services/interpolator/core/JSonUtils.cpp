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

#include "../../../core/data-model/Filter.hpp"
#include "../../../core/utility/GeoUtils.hpp"
#include "../../../core/utility/JSonUtils.hpp"
#include "../../../core/utility/Logger.hpp"
#include "InterpolatorParams.hpp"

// TerraLib
//#include <terralib/geometry/Point.h>
//#include <terralib/geometry/WKTWriter.h>

// Qt
#include <QJsonDocument>
#include <QJsonArray>


// -------------------------------------------
// Utility functions section
// -------------------------------------------
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

  QJsonArray ll,
      ur;

  ll.append(env.getLowerLeftX());
  ll.append(env.getLowerLeftY());
  ur.append(env.getUpperRightX());
  ur.append(env.getUpperRightY());

  obj.insert("ll_corner", ll);
  obj.insert("ur_corner", ur);

  return obj;
}

/*!
 * \brief Returns the specialized parameters giving the \a type.
 *
 * \param type Type of interpolator to be used.
 *
 * \return A specialized parameters object.
 */
terrama2::services::interpolator::core::InterpolatorParams* GetParameters(const int& type)
{
  switch (type)
  {
    case terrama2::services::interpolator::core::NEARESTNEIGHBOR:
      return new terrama2::services::interpolator::core::NNInterpolatorParams;

    case terrama2::services::interpolator::core::AVGDIST:
      return new terrama2::services::interpolator::core::AvgDistInterpolatorParams;

    case terrama2::services::interpolator::core::SQRAVGDIST:
      return new terrama2::services::interpolator::core::SqrAvgDistInterpolatorParams;

    default:
      return 0;
  }
}

/*!
 * \overload terrama2::services::interpolator::core::InterpolatorParams* GetParameters(const QString& type)
 *
 * \param type Type as string, this comes from the JSon file. There are three available values: "NEAREST-NEIGHBOR", "AVERAGE-NEIGHBOR" and "W-AVERAGE-NEIGHBOR".
 *
 * \exception If \a type is different of these, previously presented, a terrama2::core::JSonParserException will be raised.
 */
terrama2::services::interpolator::core::InterpolatorParams* GetParameters(const QString& type)
{
  if(type.compare("NEAREST-NEIGHBOR", Qt::CaseInsensitive))
    return GetParameters(terrama2::services::interpolator::core::NEARESTNEIGHBOR);

  if(type.compare("AVERAGE-NEIGHBOR", Qt::CaseInsensitive))
    return GetParameters(terrama2::services::interpolator::core::AVGDIST);

  if(type.compare("W-AVERAGE-NEIGHBOR", Qt::CaseInsensitive))
    return GetParameters(terrama2::services::interpolator::core::SQRAVGDIST);

  QString errMsg = QObject::tr("Invalid Interpolator type.");
  TERRAMA2_LOG_ERROR() << errMsg;
  throw terrama2::core::JSonParserException() << terrama2::ErrorDescription(errMsg);
}

/*!
 * \brief Return a string that represents the \a type.
 *
 * \param type Type of the interpolator. See terrama2::services::interpolator::core::InterpolatorType documentation for details.
 *
 * \return One of the above: "NEAREST-NEIGHBOR", "AVERAGE-NEIGHBOR" or "W-AVERAGE-NEIGHBOR".
 *
 * \exception If type is diferent of the specified in terrama2::services::interpolator::core::InterpolatorType enumerator, a terrama2::core::JSonParserException will be raised.
 */
QString GetInterpolatorAsString(const int& type)
{
  switch (type)
  {
    case terrama2::services::interpolator::core::NEARESTNEIGHBOR:
      return "NEAREST-NEIGHBOR";

    case terrama2::services::interpolator::core::AVGDIST:
      return "AVERAGE-NEIGHBOR";

    case terrama2::services::interpolator::core::SQRAVGDIST:
      return "W-AVERAGE-NEIGHBOR";
  }

  QString errMsg = QObject::tr("Invalid Interpolator type.");
  TERRAMA2_LOG_ERROR() << errMsg;
  throw terrama2::core::JSonParserException() << terrama2::ErrorDescription(errMsg);
}

// -------------------------------------------
// End Utilities functions section
// -------------------------------------------

terrama2::services::interpolator::core::InterpolatorParamsPtr terrama2::services::interpolator::core::fromInterpolatorJson(QJsonObject json, terrama2::core::DataManager*)
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
       && json.contains("input_data_series")
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

  QString interpolatorType = json["interpolator_strategy"].toString();

  res = GetParameters(interpolatorType);

  res->id_ = static_cast<uint32_t>(json["id"].toInt());
  res->projectId_ = static_cast<uint32_t>(json["project_id"].toInt());
  res->serviceInstanceId_ = static_cast<uint32_t>(json["service_instance_id"].toInt());

  res->series_ = static_cast<uint32_t>(json["input_data_series"].toInt());
  res->outSeries_ = static_cast<uint32_t>(json["output_data_series"].toInt());

  res->filter_ = terrama2::core::Filter();
//  res->filter_ = terrama2::core::fromFilterJson(json["filter"].toObject(), dataManager);
  res->active_ = json["active"].toString().toLower() == "true" ?
                      true :
                      false;

  res->interpolationType_ = res->interpolationType_;

  res->attributeName_ = json["interpolation_attribute"].toString().toStdString();
  res->resolutionX_ = json["resolution_x"].toDouble();
  res->resolutionY_ = json["resolution_y"].toDouble();
  res->srid_ = json["srid"].toInt();
  res->numNeighbors_ = static_cast<size_t>(json["number_of_neighbors"].toInt());

  auto bboxObj = json["bounding_rect"].toObject();

  auto llArray = bboxObj["ll_corner"].toArray();

  auto urArray = bboxObj["ur_corner"].toArray();

  double x1 = llArray[0].toDouble(),
      y1 = llArray[1].toDouble(),
      x2 = urArray[0].toDouble(),
      y2 = urArray[1].toDouble();

  te::gm::Envelope env(x1, y1, x2, y2);

  res->bRect_ = env;

  if(res->interpolationType_ == terrama2::services::interpolator::core::SQRAVGDIST)
  {
    terrama2::services::interpolator::core::SqrAvgDistInterpolatorParams* auxPar = dynamic_cast<terrama2::services::interpolator::core::SqrAvgDistInterpolatorParams*>(res);

    if(auxPar == 0)
    {
      QString errMsg = QObject::tr("Invalid Weighted Interpolator JSON object.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
    }

    auxPar->pow_ = json["power_factor"].toInt();
  }

  return InterpolatorParamsPtr(res);
}

QJsonObject terrama2::services::interpolator::core::toJson(InterpolatorParamsPtr params)
{
  QJsonObject obj;
  obj.insert("class", QString("Interpolator"));
  obj.insert("id", static_cast<int32_t>(params->id_));
  obj.insert("project_id", static_cast<int32_t>(params->projectId_));
  obj.insert("service_instance_id", static_cast<int32_t>(params->serviceInstanceId_));
  obj.insert("input_data_series", static_cast<int32_t>(params->series_));
  obj.insert("output_data_series", static_cast<int32_t>(params->outSeries_));
  obj.insert("interpolator_strategy", GetInterpolatorAsString(params->interpolationType_));
  obj.insert("number_of_neighbors", static_cast<int32_t>(params->numNeighbors_));
  obj.insert("resolution_x", static_cast<double>(params->resolutionX_));
  obj.insert("resolution_y", static_cast<double>(params->resolutionY_));
  obj.insert("interpolation_attribute", QString::fromStdString(params->attributeName_));
  obj.insert("srid", static_cast<int32_t>(params->srid_));
  obj.insert("active", params->active_);

  if(params->interpolationType_ == terrama2::services::interpolator::core::SQRAVGDIST)
  {
    terrama2::services::interpolator::core::SqrAvgDistInterpolatorParams* auxPar = dynamic_cast<terrama2::services::interpolator::core::SqrAvgDistInterpolatorParams*>(params.get());

    if(auxPar == 0)
    {
      QString errMsg = QObject::tr("Invalid Weighted Interpolator JSON object.");
      TERRAMA2_LOG_ERROR() << errMsg;
      throw terrama2::core::JSonParserException() << ErrorDescription(errMsg);
    }

    obj.insert("power_factor", static_cast<int32_t>(auxPar->pow_));
  }

  obj.insert("bounding_rect", GetBoundingRect(params->bRect_));

  return obj;
}
