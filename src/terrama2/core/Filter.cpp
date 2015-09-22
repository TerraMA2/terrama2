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
#include <terralib/geometry.h>


terrama2::core::Filter::Filter(DataSetItemPtr dataSetItemPtr)
  : dataSetItemPtr_(dataSetItemPtr),
    byValue_(0.0),
    byValueType_(NONE_TYPE)
{

}

terrama2::core::Filter::~Filter()
{

}

terrama2::core::DataSetItemPtr terrama2::core::Filter::dataSetItemPtr() const
{
  return dataSetItemPtr_;
}

void terrama2::core::Filter::setDataSetItemPtr(const terrama2::core::DataSetItemPtr& dataSetItemPtr)
{
  dataSetItemPtr_ = dataSetItemPtr;
}

std::shared_ptr<te::dt::DateTime> terrama2::core::Filter::discardBefore()
{
  return discardBefore_;
}

void terrama2::core::Filter::setDiscardBefore(std::shared_ptr<te::dt::DateTime> discardBefore)
{
  discardBefore_ = discardBefore;
}

std::shared_ptr<te::dt::DateTime> terrama2::core::Filter::discardAfter()
{
  return discardAfter_;
}

void terrama2::core::Filter::setDiscardAfter(std::shared_ptr<te::dt::DateTime> discardAfter)
{
  discardAfter_ = discardAfter;
}

std::shared_ptr<te::gm::Geometry> terrama2::core::Filter::geometry()
{
  return geometry_;
}

void terrama2::core::Filter::setGeometry(std::shared_ptr<te::gm::Geometry> geometry)
{
  geometry_ = geometry;
}

double terrama2::core::Filter::byValue() const
{
  return byValue_;
}

void terrama2::core::Filter::setByValue(const double byValue)
{
  byValue_ = byValue;
}

terrama2::core::Filter::ByValueType terrama2::core::Filter::byValueType() const
{
  return byValueType_;
}

void terrama2::core::Filter::setByValueType(const terrama2::core::Filter::ByValueType byValueType)
{
  byValueType_ = byValueType;
}

std::string terrama2::core::Filter::bandFilter() const
{
  return bandFilter_;
}

void terrama2::core::Filter::setBandFilter(const std::string& bandFilter)
{
  bandFilter_ = bandFilter;
}
