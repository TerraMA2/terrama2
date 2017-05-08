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
  \file terrama2/core/utility/JoinDataSet.cpp

  \brief Join two te::da::DataSet

  \author Jano Simas
*/

#include "JoinDataSet.hpp"

#include <vector>
#include <limits>

#include <boost/range/algorithm.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/join.hpp>

#include <terralib/dataaccess/utils/Utils.h>

terrama2::core::JoinDataSet::JoinDataSet(std::unique_ptr<te::da::DataSetType> ds1Type,
                                         std::unique_ptr<te::da::DataSet> ds1,
                                         const std::string& ds1Attribute,
                                         std::unique_ptr<te::da::DataSetType> ds2Type,
                                         std::unique_ptr<te::da::DataSet> ds2,
                                         const std::string& ds2Attribute)
  : te::da::DataSet(),
    ds1Type_(std::move(ds1Type)),
    ds1_(std::move(ds1)),
    ds1Attribute_(ds1Attribute),
    ds2Type_(std::move(ds2Type)),
    ds2_(std::move(ds2)),
    ds2Attribute_(ds2Attribute)
{
  if(ds1_->getTraverseType() != te::common::RANDOM || ds2_->getTraverseType() != te::common::RANDOM)
  {
    // cannot join DataSet's that are not random access
    // We need to be able to look for the joined line in the DataSet's
    throw;//TODO: throw wrong dataset type
  }

  fillProperties(ds1Type_, ds2Type_);
  fillKeyMap(ds1_, ds1Attribute_, ds2_, ds2Attribute_);
}

//! Fill the map of ds1Key to a ds2 line.
void terrama2::core::JoinDataSet::fillKeyMap(const std::unique_ptr<te::da::DataSet>& ds1, const std::string& ds1Attribute, const std::unique_ptr<te::da::DataSet>& ds2, const std::string& ds2Attribute)
{
  auto ds1Size = ds1->size();
  for(int i = 0; i < ds1Size; ++i)
  {
    ds1->move(i);
    auto key1 = ds1->getAsString(ds1Attribute);

    auto ds2Size = ds2->size();
    for(int j = 0; j < ds2Size; ++j)
    {
      ds2->move(j);
      auto key2 = ds2->getAsString(ds2Attribute);

      // compare the two key attributes
      // the key as tring will be used in the map too,
      // we need some way to compare abstract data
      if(key1 == key2)
      {
        // create a map of ds1 key to ds2 line
        keyLineMap_.emplace(key1, j);
      }
    }
  }
}

void terrama2::core::JoinDataSet::fillProperties(const std::unique_ptr<te::da::DataSetType>& ds1Type, const std::unique_ptr<te::da::DataSetType>& ds2Type)
{
  auto ds1Properties = ds1Type->getProperties();
  auto ds2Properties = ds2Type->getProperties();

  // clone ds1 properties
  auto ds1PropertiesTransformed = ds1Properties | boost::adaptors::transformed(std::mem_fn(&te::dt::Property::clone));

  // function to filter ds2 properties with the same name as ds1 properties
  auto not_contains = [&ds1Properties](te::dt::Property* prop2) -> bool
                      {
                        auto it = boost::range::find_if(ds1Properties, [&prop2](te::dt::Property* prop1)
                        {
                          return prop1->getName() == prop2->getName();
                        });
                        return it == ds1Properties.end();
                      };

  // filter and clone ds2 properties
  auto ds2PropertiesFiltered = ds2Properties | boost::adaptors::filtered(std::ref(not_contains)) | boost::adaptors::transformed(std::mem_fn(&te::dt::Property::clone));
  // join ds1 and ds2 properties
  auto newProperties = boost::join(ds1PropertiesTransformed, ds2PropertiesFiltered);

  // assign to internal properties vector
  properties_.assign(boost::begin(newProperties), boost::end(newProperties));
}

std::auto_ptr<te::gm::Envelope> terrama2::core::JoinDataSet::getExtent(std::size_t i)
{
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getExtent(pos);
}

char terrama2::core::JoinDataSet::getChar(std::size_t i) const
{
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getChar(pos);
}

unsigned char terrama2::core::JoinDataSet::getUChar(std::size_t i) const
{
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getUChar(pos);
}

boost::int16_t terrama2::core::JoinDataSet::getInt16(std::size_t i) const
{
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getInt16(pos);
}

boost::int32_t terrama2::core::JoinDataSet::getInt32(std::size_t i) const
{
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getInt32(pos);
}

boost::int64_t terrama2::core::JoinDataSet::getInt64(std::size_t i) const
{
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getInt64(pos);
}

bool terrama2::core::JoinDataSet::getBool(std::size_t i) const
{
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getBool(pos);
}

float terrama2::core::JoinDataSet::getFloat(std::size_t i) const
{
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getFloat(pos);
}

double terrama2::core::JoinDataSet::getDouble(std::size_t i) const
{
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getDouble(pos);
}

std::string terrama2::core::JoinDataSet::getNumeric(std::size_t i) const
{
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getNumeric(pos);
}

std::string terrama2::core::JoinDataSet::getString(std::size_t i) const
{
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getString(pos);
}

std::auto_ptr<te::dt::ByteArray> terrama2::core::JoinDataSet::getByteArray(std::size_t i) const
{
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getByteArray(pos);
}

std::auto_ptr<te::gm::Geometry> terrama2::core::JoinDataSet::getGeometry(std::size_t i) const
{
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getGeometry(pos);
}

std::auto_ptr<te::rst::Raster> terrama2::core::JoinDataSet::getRaster(std::size_t i) const
{
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getRaster(pos);
}

std::auto_ptr<te::dt::DateTime> terrama2::core::JoinDataSet::getDateTime(std::size_t i) const
{
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getDateTime(pos);
}

std::auto_ptr<te::dt::Array> terrama2::core::JoinDataSet::getArray(std::size_t i) const
{
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getArray(pos);
}

bool terrama2::core::JoinDataSet::isNull(std::size_t i) const
{
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->isNull(pos);
}

char terrama2::core::JoinDataSet::getChar(const std::string& name) const
{
  auto i = getLocalPropertyPos(name);
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getChar(pos);
}

unsigned char terrama2::core::JoinDataSet::getUChar(const std::string& name) const
{
  auto i = getLocalPropertyPos(name);
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getUChar(pos);
}

boost::int16_t terrama2::core::JoinDataSet::getInt16(const std::string& name) const
{
  auto i = getLocalPropertyPos(name);
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getInt16(pos);
}

boost::int32_t terrama2::core::JoinDataSet::getInt32(const std::string& name) const
{
  auto i = getLocalPropertyPos(name);
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getInt32(pos);
}

boost::int64_t terrama2::core::JoinDataSet::getInt64(const std::string& name) const
{
  auto i = getLocalPropertyPos(name);
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getInt64(pos);
}

bool terrama2::core::JoinDataSet::getBool(const std::string& name) const
{
  auto i = getLocalPropertyPos(name);
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getBool(pos);
}

float terrama2::core::JoinDataSet::getFloat(const std::string& name) const
{
  auto i = getLocalPropertyPos(name);
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getFloat(pos);
}

double terrama2::core::JoinDataSet::getDouble(const std::string& name) const
{
  auto i = getLocalPropertyPos(name);
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getDouble(pos);
}

std::string terrama2::core::JoinDataSet::getNumeric(const std::string& name) const
{
  auto i = getLocalPropertyPos(name);
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getNumeric(pos);
}

std::string terrama2::core::JoinDataSet::getString(const std::string& name) const
{
  auto i = getLocalPropertyPos(name);
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getString(pos);
}

std::auto_ptr<te::dt::ByteArray> terrama2::core::JoinDataSet::getByteArray(const std::string& name) const
{
  auto i = getLocalPropertyPos(name);
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getByteArray(pos);
}

std::auto_ptr<te::gm::Geometry> terrama2::core::JoinDataSet::getGeometry(const std::string& name) const
{
  auto i = getLocalPropertyPos(name);
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getGeometry(pos);
}

std::auto_ptr<te::rst::Raster> terrama2::core::JoinDataSet::getRaster(const std::string& name) const
{
  auto i = getLocalPropertyPos(name);
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getRaster(pos);
}

std::auto_ptr<te::dt::DateTime> terrama2::core::JoinDataSet::getDateTime(const std::string& name) const
{
  auto i = getLocalPropertyPos(name);
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getDateTime(pos);
}

std::auto_ptr<te::dt::Array> terrama2::core::JoinDataSet::getArray(const std::string& name) const
{
  auto i = getLocalPropertyPos(name);
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->getArray(pos);
}

bool terrama2::core::JoinDataSet::isNull(const std::string& name) const
{
  auto i = getLocalPropertyPos(name);
  auto& dataset = getDataSet(i);
  auto pos = getPropertyPos(i);

  return dataset->isNull(pos);
}

std::size_t terrama2::core::JoinDataSet::getLocalPropertyPos(const std::string& name) const
{
  for(auto i = 0; i < properties_.size(); ++i)
  {
    if(properties_.at(i)->getName() == name)
      return i;
  }

  return std::numeric_limits<std::size_t>::max();
}

std::size_t terrama2::core::JoinDataSet::getPropertyPos(std::size_t i) const
{
  if(i < ds1_->getNumProperties())
  {
    return i;
  }
  else
  {
    auto property = properties_.at(i);
    auto pos = te::da::GetPropertyPos(ds2_.get(), property->getName());

    return pos;
  }
}

std::unique_ptr<te::da::DataSet>& terrama2::core::JoinDataSet::getDataSet(std::size_t i) const
{
  if(i < ds1_->getNumProperties())
  {
    return ds1_;
  }
  else
  {
    auto key = ds1_->getAsString(ds1Attribute_);
    auto line = keyLineMap_.at(key);

    ds2_->move(line);
    return ds2_;
  }
}
