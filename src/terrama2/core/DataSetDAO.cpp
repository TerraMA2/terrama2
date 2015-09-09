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
  \file terrama2/core/DataSetDAO.hpp

  \brief DataSet DAO...

  \author Paulo R. M. Oliveira
*/

#include "DataSetDAO.hpp"
#include "DataSet.hpp"

// STL
#include <vector>

// terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/query/Query.h>

terrama2::core::DataSetDAO::DataSetDAO(std::auto_ptr<te::da::DataSourceTransactor> transactor)
  : transactor_(transactor)
{

}

terrama2::core::DataSetDAO::~DataSetDAO()
{

}


bool terrama2::core::DataSetDAO::save(terrama2::core::DataSetPtr dataSet)
{
  return false;
}


bool terrama2::core::DataSetDAO::update(terrama2::core::DataSetPtr dataSet)
{
  return false;
}


bool terrama2::core::DataSetDAO::remove(const int &id)
{
  return false;
}


terrama2::core::DataSetPtr terrama2::core::DataSetDAO::get(const int &id) const
{
  return terrama2::core::DataSetPtr(0);
}

std::vector<terrama2::core::DataSetPtr> terrama2::core::DataSetDAO::list() const
{
  std::vector<terrama2::core::DataSetPtr> vecCollectors;
  return vecCollectors;
}
