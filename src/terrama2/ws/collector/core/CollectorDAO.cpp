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
  \file terrama2/ws/collector/core/CollectorDAO.hpp

  \brief Collector DAO...

  \author Paulo R. M. Oliveira
*/

#include "CollectorDAO.hpp"
#include "Collector.hpp"

// STL
#include <vector>

// terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/query/Query.h>

terrama2::ws::collector::core::CollectorDAO::CollectorDAO(std::auto_ptr<te::da::DataSourceTransactor> transactor)
  : transactor_(transactor)
{

}

terrama2::ws::collector::core::CollectorDAO::~CollectorDAO()
{

}


bool terrama2::ws::collector::core::CollectorDAO::save(terrama2::ws::collector::core::Collector *collector)
{

  return false;
}


bool terrama2::ws::collector::core::CollectorDAO::update(terrama2::ws::collector::core::Collector *collector)
{
  return false;
}


bool terrama2::ws::collector::core::CollectorDAO::remove(const int &id)
{
  return false;
}


terrama2::ws::collector::core::Collector* terrama2::ws::collector::core::CollectorDAO::get(const int &id) const
{
  return 0;
}

std::vector<terrama2::ws::collector::core::Collector *> terrama2::ws::collector::core::CollectorDAO::list(const int &id) const
{
  std::vector<terrama2::ws::collector::core::Collector*> vecCollectors;
  return vecCollectors;
}
