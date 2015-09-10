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

#include "DataProvider.hpp"

terrama2::core::DataSet::DataSet(DataProviderPtr dataProvider)
  : dataProvider_(dataProvider)
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
