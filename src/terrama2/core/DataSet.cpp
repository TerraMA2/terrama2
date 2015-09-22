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
  \file terrama2/core/DataSet.cpp

  \brief Metadata about a given dataset.

  \author Gilberto Ribeiro de Queiroz
  \author Jano Simas
  \author Paulo R. M. Oliveira
  \author Vinicius Campanha
*/


// TerraMA2
#include "DataSet.hpp"
#include "DataSetItem.hpp"
#include "DataProvider.hpp"

terrama2::core::DataSet::DataSet(DataProviderPtr dataProvider, const std::string& name, Kind kind, const uint64_t id)
  : id_(id),
    name_(name),
    status_(INACTIVE),
    dataProvider_(dataProvider),
    kind_(kind),
    dataFrequency_(0, 0, 0),
    schedule_(0, 0, 0),
    scheduleRetry_(0, 0, 0),
    scheduleTimeout_(0, 0, 0)
{

}

terrama2::core::DataSet::~DataSet()
{

}

uint64_t terrama2::core::DataSet::id() const
{
  return id_;
}

void terrama2::core::DataSet::setId(uint64_t id)
{
  id_ = id;
}

std::string terrama2::core::DataSet::name() const
{
  return name_;
}

void terrama2::core::DataSet::setName(const std::string &name)
{
  name_ = name;
}

std::string terrama2::core::DataSet::description() const
{
  return description_;
}

void terrama2::core::DataSet::setDescription(const std::string &description)
{
  description_ = description;
}

terrama2::core::DataSet::Kind terrama2::core::DataSet::kind() const
{
  return kind_;
}

void terrama2::core::DataSet::setKind(const terrama2::core::DataSet::Kind &kind)
{
  kind_ = kind;
}

terrama2::core::DataSet::Status terrama2::core::DataSet::status() const
{
  return status_;
}

void terrama2::core::DataSet::setStatus(const terrama2::core::DataSet::Status &status)
{
  status_ = status;
}

terrama2::core::DataProviderPtr terrama2::core::DataSet::dataProvider() const
{
  return dataProvider_;
}

te::dt::TimeDuration terrama2::core::DataSet::dataFrequency() const
{
  return dataFrequency_;
}

void terrama2::core::DataSet::setDataFrequency(const te::dt::TimeDuration &dataFrequency)
{
  dataFrequency_ = dataFrequency;
}

te::dt::TimeDuration terrama2::core::DataSet::schedule() const
{
  return schedule_;
}

void terrama2::core::DataSet::setSchedule(const te::dt::TimeDuration &schedule)
{
  schedule_ = schedule;
}

te::dt::TimeDuration terrama2::core::DataSet::scheduleRetry() const
{
  return scheduleRetry_;
}

void terrama2::core::DataSet::setScheduleRetry(const te::dt::TimeDuration &scheduleRetry)
{
  scheduleRetry_ = scheduleRetry;
}

te::dt::TimeDuration terrama2::core::DataSet::scheduleTimeout() const
{
  return scheduleTimeout_;
}

void terrama2::core::DataSet::setScheduleTimeout(const te::dt::TimeDuration &scheduleTimeout)
{
  scheduleTimeout_ = scheduleTimeout;
}

std::map<std::string, std::string> terrama2::core::DataSet::metadata() const
{
  return metadata_;
}

void terrama2::core::DataSet::setMetadata(const std::map<std::string, std::string>& metadata)
{
  metadata_ = metadata;
}

std::vector<terrama2::core::DataSet::CollectRule> terrama2::core::DataSet::collectRules() const
{
  return collectRules_;
}

void terrama2::core::DataSet::setCollectRules(const std::vector<terrama2::core::DataSet::CollectRule>& collectRules)
{
  collectRules_ = collectRules;
}

std::vector<terrama2::core::DataSetItemPtr> terrama2::core::DataSet::dataSetItemList() const
{
  return dataSetItemList_;
}

void terrama2::core::DataSet::setDataSetItemList(const std::vector<terrama2::core::DataSetItemPtr>& dataSetItemList)
{
  dataSetItemList_ = dataSetItemList;
}

