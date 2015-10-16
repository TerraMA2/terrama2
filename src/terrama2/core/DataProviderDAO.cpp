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

// TerraMA2
#include "DataProviderDAO.hpp"
#include "DataSetDAO.hpp"
#include "Exception.hpp"
#include "Utils.hpp"

// TerraLib
#include <terralib/dataaccess/datasource/DataSourceTransactor.h>

// Qt
#include <QObject>

//Boost
#include <boost/format.hpp>

void terrama2::core::DataProviderDAO::save(DataProvider& provider,
                                           te::da::DataSourceTransactor& transactor,
                                           const bool shallow)
{
  if(provider.id() != 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not save a data provider with identifier different than 0."));

  try
  {
    boost::format query("INSERT INTO terrama2.data_provider (name, description, kind, uri, active) VALUES('%1%', '%2%', %3%, '%4%', %5%)");

    query.bind_arg(1, provider.name());
    query.bind_arg(2, provider.description());
    query.bind_arg(3, (int)provider.kind());
    query.bind_arg(4, provider.uri());
    query.bind_arg(5, ToString(ToBool(provider.status())));

    transactor.execute(query.str());

    provider.setId(transactor.getLastGeneratedId());

    if(shallow)
      return;

    for(auto& dataset: provider.datasets())
      DataSetDAO::save(dataset, transactor);
  }
  catch(const terrama2::Exception&)
  {
    throw;
  }
  catch(const std::exception& e)
  {
    throw DataAccessError() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not save the data provider."));
  }
}

void terrama2::core::DataProviderDAO::update(DataProvider& provider,
                                             te::da::DataSourceTransactor& transactor,
                                             const bool shallow)
{
  if(provider.id() == 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not update a data provider with identifier: 0."));

  try
  {
    boost::format query("UPDATE terrama2.data_provider SET name = '%1%', description = '%2%', kind = %3%, uri = '%4%', active = %5% WHERE id = %6%");

    query.bind_arg(1, provider.name());
    query.bind_arg(2, provider.description());
    query.bind_arg(3, (int)provider.kind());
    query.bind_arg(4, provider.uri());
    query.bind_arg(5, ToString(ToBool(provider.status())));
    query.bind_arg(6, provider.id());

    transactor.execute(query.str());

    if(shallow)
      return;

// TODO: Verificar os existentes no banco pra ver oq precisa inserir/remove/atualizar
    for(auto& dataset: provider.datasets())
      DataSetDAO::update(dataset, transactor);
  }
  catch(const terrama2::Exception&)
  {
    throw;
  }
  catch(const std::exception& e)
  {
    throw DataAccessError() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not update the data provider."));
  }
}

void terrama2::core::DataProviderDAO::remove(const uint64_t id, te::da::DataSourceTransactor& transactor)
{
  if(id == 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not remove a data provider with identifier: 0."));

  try
  {
    boost::format query("DELETE FROM terrama2.data_provider WHERE id = %1%");
    query.bind_arg(1, id);

    transactor.execute(query.str());
  }
  catch(const terrama2::Exception&)
  {
    throw;
  }
  catch(const std::exception& e)
  {
    throw DataAccessError() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not remove the data provider."));
  }
}


terrama2::core::DataProvider
terrama2::core::DataProviderDAO::load(const uint64_t id, te::da::DataSourceTransactor& transactor)
{
  if(id == 0)
    throw InvalidArgumentError() << ErrorDescription(QObject::tr("Can not load a data provider with identifier: 0."));

  try
  {
    boost::format query("SELECT * FROM terrama2.data_provider WHERE id = %1%");
    query.bind_arg(1, id);

    std::auto_ptr<te::da::DataSet> provider_result = transactor.query(query.str());

    if(provider_result->moveNext())
    {
      DataProvider provider;
      provider.setKind(ToDataProviderKind(provider_result->getInt32("kind")));
      provider.setName(provider_result->getAsString("name"));
      provider.setId(provider_result->getInt32("id"));
      provider.setDescription(provider_result->getString("description"));
      provider.setUri(provider_result->getString("uri"));
      provider.setStatus(ToDataProviderStatus(provider_result->getBool("active")));

      std::vector<DataSet> datasets = DataSetDAO::loadAll(id, transactor);
      
      for(const auto& dataset : datasets)
        provider.add(dataset);

      return provider;
    }
  }
  catch(const terrama2::Exception&)
  {
    throw;
  }
  catch(const std::exception& e)
  {
    throw DataAccessError() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessError() << ErrorDescription(QObject::tr("Could not remove the data provider."));
  }

  return DataProvider();
}

std::vector<terrama2::core::DataProvider>
terrama2::core::DataProviderDAO::loadAll(te::da::DataSourceTransactor& transactor)
{
  std::vector<DataProvider> providers;
  
  try
  {
    std::auto_ptr<te::da::DataSet> provider_result = transactor.getDataSet("terrama2.data_provider");
    
    while(provider_result->moveNext())
    {
      DataProvider provider;
      
      provider.setKind(ToDataProviderKind(provider_result->getInt32("kind")));
      provider.setName(provider_result->getAsString("name"));
      provider.setId(provider_result->getInt32("id"));
      provider.setDescription(provider_result->getString("description"));
      provider.setUri(provider_result->getString("uri"));
      provider.setStatus(ToDataProviderStatus(provider_result->getBool("active")));
      
      std::vector<DataSet> datasets = DataSetDAO::loadAll(provider.id(), transactor);
      
      for(const auto& dataset : datasets)
        provider.add(dataset);
      
      providers.push_back(std::move(provider));
    }
  }
  catch(const terrama2::Exception&)
  {
    throw;
  }
  catch(const std::exception& e)
  {
    throw DataAccessError() << ErrorDescription(e.what());
  }
  catch(...)
  {
    throw DataAccessError() << ErrorDescription(QObject::tr("Unexpected error loading data providers."));
  }
  
  return std::move(providers);
}
