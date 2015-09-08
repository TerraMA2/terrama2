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
  \file terrama2/core/DataProviderDAO.hpp

  \brief DataProvider DAO...

  \author Paulo R. M. Oliveira
*/

#include "DataProviderDAO.hpp"
#include "DataProvider.hpp"

// STL
#include <vector>
#include <memory>

// terralib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>
#include <terralib/dataaccess/query/Query.h>
#include <terralib/common/StringUtils.h>

terrama2::core::DataProviderDAO::DataProviderDAO(std::auto_ptr<te::da::DataSourceTransactor> transactor)
  : transactor_(transactor)
{

}

terrama2::core::DataProviderDAO::~DataProviderDAO()
{

}


bool terrama2::core::DataProviderDAO::save(terrama2::core::DataProviderPtr dataProvider)
{
  try
  {
    transactor_->begin();

    std::auto_ptr<te::da::DataSet> dataset = transactor_->getDataSet("data_provider");

  }
  catch(...)
  {

  }

  return true;
}


bool terrama2::core::DataProviderDAO::update(terrama2::core::DataProviderPtr dataProvider)
{
  return false;
}


bool terrama2::core::DataProviderDAO::remove(const int &id)
{
  return false;
}


terrama2::core::DataProviderPtr terrama2::core::DataProviderDAO::get(const int &id) const
{
  return 0;
}

std::vector<terrama2::core::DataProviderPtr> terrama2::core::DataProviderDAO::list() const
{
  std::vector<terrama2::core::DataProviderPtr> vecCollectors;
  return vecCollectors;
}
