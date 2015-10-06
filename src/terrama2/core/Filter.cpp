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
  \file terrama2/core/Filter.cpp

  \brief Contains the filters to be applied in a dataset item.

  \author Paulo R. M. Oliveira
*/

// TerraMA2
#include "Filter.hpp"
#include "DataSetItem.hpp"

// TerraLib
#include <terralib/datatype/DateTime.h>
#include <terralib/geometry/Geometry.h>

terrama2::core::Filter::Filter(const DataSetItemPtr& item)
  : datasetItem_(item),
    value_(0.0),
    expressionType_(NONE_TYPE)
{

}

terrama2::core::Filter::~Filter()
{

}

terrama2::core::DataSetItemPtr terrama2::core::Filter::dataSetItem() const
{
  return datasetItem_;
}

void terrama2::core::Filter::setDataSetItemPtr(const terrama2::core::DataSetItemPtr& datasetItem)
{
  datasetItem_ = datasetItem;
}

const te::dt::DateTime* terrama2::core::Filter::discardBefore() const
{
  return discardBefore_.get();
}

void terrama2::core::Filter::setDiscardBefore(std::unique_ptr<te::dt::DateTime> discardBefore)
{
  discardBefore_ = std::move(discardBefore);
}

const te::dt::DateTime* terrama2::core::Filter::discardAfter() const
{
  return discardAfter_.get();
}

void terrama2::core::Filter::setDiscardAfter(std::unique_ptr<te::dt::DateTime> discardAfter)
{
  discardAfter_ = std::move(discardAfter);
}

const te::gm::Geometry* terrama2::core::Filter::geometry() const
{
  return geometry_.get();
}

void terrama2::core::Filter::setGeometry(std::unique_ptr<te::gm::Geometry> geom)
{
  geometry_ = std::move(geom);
}

double terrama2::core::Filter::value() const
{
  return value_;
}

void terrama2::core::Filter::setValue(const double v)
{
  value_ = v;
}

terrama2::core::Filter::ExpressionType terrama2::core::Filter::expressionType() const
{
  return expressionType_;
}

void terrama2::core::Filter::setExpressionType(const ExpressionType t)
{
  expressionType_ = t;
}

std::string terrama2::core::Filter::bandFilter() const
{
  return bandFilter_;
}

void terrama2::core::Filter::setBandFilter(const std::string& bandFilter)
{
  bandFilter_ = bandFilter;
}
