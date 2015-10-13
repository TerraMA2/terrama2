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
  \file terrama2/core/DataProvider.cpp

  \brief Models the information of a data provider (or data server).

  \author Gilberto Ribeiro de Queiroz
  \author Jano Simas
  \author Paulo R. M. Oliveira
  \author Vinicius Campanha
*/

// TerraMA2
#include "DataProvider.hpp"
#include "DataSet.hpp"

terrama2::core::DataProvider::DataProvider(const uint64_t id, Kind k)
  : id_(id),
    kind_(k),
    status_(INACTIVE)
{
}

terrama2::core::DataProvider::~DataProvider()
{
}

uint64_t terrama2::core::DataProvider::id() const
{
  return id_;
}

void terrama2::core::DataProvider::setId(uint64_t id)
{
  id_ = id;
}

const std::string&
terrama2::core::DataProvider::name() const
{
  return name_;
}

void terrama2::core::DataProvider::setName(const std::string& name)
{
  name_ = name;
}

const std::string&
terrama2::core::DataProvider::description() const
{
  return description_;
}

void terrama2::core::DataProvider::setDescription(const std::string& description)
{
  description_ = description;
}

terrama2::core::DataProvider::Kind
terrama2::core::DataProvider::kind() const
{
  return kind_;
}

void terrama2::core::DataProvider::setKind(Kind k)
{
  kind_ = k;
}

const std::string&
terrama2::core::DataProvider::uri() const
{
  return uri_;
}

void terrama2::core::DataProvider::setUri(const std::string& uri)
{
  uri_ = uri;
}

terrama2::core::DataProvider::Status
terrama2::core::DataProvider::status() const
{
  return status_;
}

void
terrama2::core::DataProvider::setStatus(Status s)
{
  status_ = s;
}

const std::vector<terrama2::core::DataSetPtr>&
terrama2::core::DataProvider::datasets() const
{
  return datasets_;
}

void
terrama2::core::DataProvider::add(std::unique_ptr<DataSet> d)
{
  datasets_.push_back(std::move(d));
}

void terrama2::core::DataProvider::remove(terrama2::core::DataSetPtr& dataset)
{
  std::remove(datasets_.begin(), datasets_.end(), dataset);
}
