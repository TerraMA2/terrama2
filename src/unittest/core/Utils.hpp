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
  \file terrama2/core/data-access/DataAccessorFile.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_UNITTEST_CORE_UTILS_HPP__
#define __TERRAMA2_UNITTEST_CORE_UTILS_HPP__

#include <terralib/dataaccess/datasource/DataSourceFactory.h>
#include <terrama2/core/utility/DataRetrieverFactory.hpp>

class DataRetrieverFactoryRaii
{
  public:

    DataRetrieverFactoryRaii(const std::string& dataProviderType,
                              const terrama2::core::DataRetrieverFactory::FactoryFnctType& f)
      : dataProviderType_(dataProviderType), f_(f)
    {
      terrama2::core::DataRetrieverFactory::getInstance().add(dataProviderType_, f_);
    }

    ~DataRetrieverFactoryRaii()
    {
      terrama2::core::DataRetrieverFactory::getInstance().remove(dataProviderType_);
    }

  private:
    std::string dataProviderType_;
    terrama2::core::DataRetrieverFactory::FactoryFnctType f_;
};

class DataSourceFactoryRaii
{
  public:
    DataSourceFactoryRaii(const std::string& type, const te::da::DataSourceFactory::FactoryFnctType& ft ) : type_(type), ft_(std::move(ft))
    {
      if(te::da::DataSourceFactory::find(type_))
      {
        te::da::DataSourceFactory::remove(type_);
        te::da::DataSourceFactory::add(type_,ft_);
      }
      else te::da::DataSourceFactory::add(type_,ft_);
    }

    ~DataSourceFactoryRaii()
    {
      te::da::DataSourceFactory::remove(type_);
    }

  private:
    std::string type_;
    te::da::DataSourceFactory::FactoryFnctType ft_;
};

#endif// __TERRAMA2_UNITTEST_CORE_UTILS_HPP__
