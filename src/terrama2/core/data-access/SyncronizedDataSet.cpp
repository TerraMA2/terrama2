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
  \file terrama2/core/data-access/SyncronizedDataSet.cpp

  \brief Thread-safe memory dataset.

  \author Paulo R. M. Oliveira
*/

#include "SyncronizedDataSet.hpp"

// TerraLib
#include <terralib/dataaccess/dataset/DataSet.h>
#include <terralib/raster/Raster.h>

terrama2::core::SyncronizedDataSet::SyncronizedDataSet(std::shared_ptr<te::da::DataSet> dataset)
: dataset_(dataset)
{
}

terrama2::core::SyncronizedDataSet::~SyncronizedDataSet()
{
}

std::shared_ptr<te::da::DataSet> terrama2::core::SyncronizedDataSet::dataset() const
{
  return dataset_;
}


std::shared_ptr<te::gm::Geometry> terrama2::core::SyncronizedDataSet::getGeometry(uint64_t row, uint64_t columnIndex) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  dataset_->move(row);
  return dataset_->getGeometry(columnIndex);
}

std::string terrama2::core::SyncronizedDataSet::getString(uint64_t row, uint64_t columnIndex) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  dataset_->move(row);
  return dataset_->getString(columnIndex);
}

double terrama2::core::SyncronizedDataSet::getDouble(uint64_t row, uint64_t columnIndex) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  dataset_->move(row);
  return dataset_->getDouble(columnIndex);
}

bool terrama2::core::SyncronizedDataSet::getBool(uint64_t row, uint64_t columnIndex) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  dataset_->move(row);
  return dataset_->getBool(columnIndex);
}

int16_t terrama2::core::SyncronizedDataSet::getInt16(uint64_t row, uint64_t columnIndex) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  dataset_->move(row);
  return dataset_->getInt16(columnIndex);
}


int32_t terrama2::core::SyncronizedDataSet::getInt32(uint64_t row, uint64_t columnIndex) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  dataset_->move(row);
  return dataset_->getInt32(columnIndex);
}


int64_t terrama2::core::SyncronizedDataSet::getInt64(uint64_t row, uint64_t columnIndex) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  dataset_->move(row);
  return dataset_->getInt64(columnIndex);
}


bool terrama2::core::SyncronizedDataSet::isNull(uint64_t row, std::size_t columnIndex) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  dataset_->move(row);
  return dataset_->isNull(columnIndex);
}


std::shared_ptr<te::rst::Raster> terrama2::core::SyncronizedDataSet::getRaster(uint64_t row, uint64_t columnIndex) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  dataset_->move(row);
  return std::shared_ptr<te::rst::Raster>(dataset_->getRaster(columnIndex));
}


std::shared_ptr<te::gm::Geometry> terrama2::core::SyncronizedDataSet::getGeometry(uint64_t row, std::string columnName) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  dataset_->move(row);
  return dataset_->getGeometry(columnName);
}

std::string terrama2::core::SyncronizedDataSet::getString(uint64_t row, std::string columnName) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  dataset_->move(row);
  return dataset_->getString(columnName);
}

double terrama2::core::SyncronizedDataSet::getDouble(uint64_t row, std::string columnName) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  dataset_->move(row);
  return dataset_->getDouble(columnName);
}

bool terrama2::core::SyncronizedDataSet::getBool(uint64_t row, std::string columnName) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  dataset_->move(row);
  return dataset_->getBool(columnName);
}

int16_t terrama2::core::SyncronizedDataSet::getInt16(uint64_t row, std::string columnName) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  dataset_->move(row);
  return dataset_->getInt16(columnName);
}


int32_t terrama2::core::SyncronizedDataSet::getInt32(uint64_t row, std::string columnName) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  dataset_->move(row);
  return dataset_->getInt32(columnName);
}


int64_t terrama2::core::SyncronizedDataSet::getInt64(uint64_t row, std::string columnName) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  dataset_->move(row);
  return dataset_->getInt64(columnName);
}


bool terrama2::core::SyncronizedDataSet::isNull(uint64_t row, std::string columnName) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  dataset_->move(row);
  return dataset_->isNull(columnName);
}

std::shared_ptr<te::rst::Raster> terrama2::core::SyncronizedDataSet::getRaster(uint64_t row, std::string columnName) const
{
  std::lock_guard<std::mutex> lock(mutex_);
  dataset_->move(row);
  return std::shared_ptr<te::rst::Raster>(dataset_->getRaster(columnName));
}


uint64_t terrama2::core::SyncronizedDataSet::size() const
{
  std::lock_guard<std::mutex> lock(mutex_);
  return dataset_->size();
}
