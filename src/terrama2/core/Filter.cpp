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

  \brief Filter information of a given dataset item.

  \author Paulo R. M. Oliveira
  \author Gilberto Ribeiro de Queiroz
*/

// TerraMA2
#include "Filter.hpp"
#include "DataSetItem.hpp"

// TerraLib
#include <terralib/datatype/DateTime.h>
#include <terralib/geometry/Geometry.h>

terrama2::core::Filter::Filter(const DataSetItem* item)
  : datasetItem_(item),
    value_(0.0),
    expressionType_(NONE_TYPE)
{

}

terrama2::core::Filter::~Filter()
{

}

terrama2::core::DataSetItemPtr terrama2::core::Filter::datasetItem() const
{
  return datasetItem_;
}

void terrama2::core::Filter::setDataSetItem(const DataSetItem* datasetItem)
{
  datasetItem_ = datasetItem;
}

const te::dt::DateTime* terrama2::core::Filter::discardBefore() const
{
  return discardBefore_.get();
}

void terrama2::core::Filter::setDiscardBefore(std::unique_ptr<te::dt::DateTime> t)
{
  discardBefore_ = std::move(t);
}

const te::dt::DateTime* terrama2::core::Filter::discardAfter() const
{
  return discardAfter_.get();
}

void terrama2::core::Filter::setDiscardAfter(std::unique_ptr<te::dt::DateTime> t)
{
  discardAfter_ = std::move(t);
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

void terrama2::core::Filter::setValue(std::unique_ptr<double> v)
{
  value_ = std::move(v);
}

terrama2::core::Filter::ExpressionType
terrama2::core::Filter::expressionType() const
{
  return expressionType_;
}

void terrama2::core::Filter::setExpressionType(const ExpressionType t)
{
  expressionType_ = t;
}

const std::string&
terrama2::core::Filter::bandFilter() const
{
  return bandFilter_;
}

void terrama2::core::Filter::setBandFilter(const std::string& f)
{
  bandFilter_ = f;
}
