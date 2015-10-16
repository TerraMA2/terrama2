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

  \brief Models the information of given dataset dataset from a data provider that should be collected by TerraMA2.

  \author Gilberto Ribeiro de Queiroz
  \author Jano Simas
  \author Paulo R. M. Oliveira
  \author Vinicius Campanha
*/


// TerraMA2
#include "DataSet.hpp"

terrama2::core::DataSet::DataSet(Kind kind, uint64_t id, uint64_t providerId)
  : kind_(kind),
    id_(id),
    provider_(providerId),
    status_(INACTIVE),
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
  
  for(auto& item : datasetItems_)
    item.setDataSet(id);

  for(auto& collectRule : collectRules_)
    collectRule.datasetId = id;
}

const std::string&
terrama2::core::DataSet::name() const
{
  return name_;
}

void terrama2::core::DataSet::setName(const std::string& name)
{
  name_ = name;
}

const std::string&
terrama2::core::DataSet::description() const
{
  return description_;
}

void terrama2::core::DataSet::setDescription(const std::string& d)
{
  description_ = d;
}

terrama2::core::DataSet::Kind terrama2::core::DataSet::kind() const
{
  return kind_;
}

void terrama2::core::DataSet::setKind(const Kind k)
{
  kind_ = k;
}

terrama2::core::DataSet::Status terrama2::core::DataSet::status() const
{
  return status_;
}

void terrama2::core::DataSet::setStatus(const Status s)
{
  status_ = s;
}

uint64_t terrama2::core::DataSet::provider() const
{
  return provider_;
}

void
terrama2::core::DataSet::setProvider(uint64_t id)
{
  provider_ = id;
}

const te::dt::TimeDuration&
terrama2::core::DataSet::dataFrequency() const
{
  return dataFrequency_;
}

void terrama2::core::DataSet::setDataFrequency(const te::dt::TimeDuration& t)
{
  dataFrequency_ = t;
}

const te::dt::TimeDuration&
terrama2::core::DataSet::schedule() const
{
  return schedule_;
}

void terrama2::core::DataSet::setSchedule(const te::dt::TimeDuration& t)
{
  schedule_ = t;
}

const te::dt::TimeDuration&
terrama2::core::DataSet::scheduleRetry() const
{
  return scheduleRetry_;
}

void terrama2::core::DataSet::setScheduleRetry(const te::dt::TimeDuration& t)
{
  scheduleRetry_ = t;
}

const te::dt::TimeDuration&
terrama2::core::DataSet::scheduleTimeout() const
{
  return scheduleTimeout_;
}

void terrama2::core::DataSet::setScheduleTimeout(const te::dt::TimeDuration& t)
{
  scheduleTimeout_ = t;
}

const std::map<std::string, std::string>&
terrama2::core::DataSet::metadata() const
{
  return metadata_;
}

void terrama2::core::DataSet::setMetadata(const std::map<std::string, std::string>& m)
{
  metadata_ = m;
}

std::vector<terrama2::core::DataSet::CollectRule>&
terrama2::core::DataSet::collectRules()
{
  return collectRules_;
}

void
terrama2::core::DataSet::setCollectRules(const std::vector<CollectRule>& rules)
{
  collectRules_ = rules;
}

std::vector<terrama2::core::DataSetItem>&
terrama2::core::DataSet::dataSetItems()
{
  return datasetItems_;
}

void
terrama2::core::DataSet::add(const DataSetItem& d)
{
  datasetItems_.push_back(d);
}

void terrama2::core::DataSet::removeDataSetItem(uint64_t id)
{
  datasetItems_.erase(std::remove_if(datasetItems_.begin(),
                                     datasetItems_.end(),
                                    [&id](const DataSetItem& item){ return (item.id() == id) ? true : false; }),
                      datasetItems_.end());
}
