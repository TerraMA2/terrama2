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
  \file terrama2/core/data-access/DataAccessorFactory.hpp

  \brief

  \author Paulo R. M. Oliveira
 */

#ifndef __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_FACTORY_HPP__
#define __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_FACTORY_HPP__

//TerraMA2
#include "../Shared.hpp"
#include "../data-model/Filter.hpp"

// TerraLib
#include <terralib/common/Singleton.h>

// STL
#include <functional>
#include <map>

namespace terrama2
{
  namespace core
  {

    struct DataSeries;
    struct DataProvider;

    /*!
      \class DataAccessorFactory

      \brief Factory class for DataAcessor.

    */
    class DataAccessorFactory : public te::common::Singleton<DataAccessorFactory>
    {
      public:

        typedef std::function<DataAccessor* (terrama2::core::DataProviderPtr dataProvider, terrama2::core::DataSeriesPtr dataSeries, terrama2::core::Filter filter)> FactoryFnctType;

        void add(const std::string& semanticName, FactoryFnctType f);

        void remove(const std::string& semanticName);

        bool find(const std::string& semanticName);

        terrama2::core::DataAccessorPtr make(terrama2::core::DataProviderPtr dataProvider, terrama2::core::DataSeriesPtr dataSeries, terrama2::core::Filter filter = Filter());

      protected:
        friend class te::common::Singleton<DataAccessorFactory>;

        DataAccessorFactory() {}
        virtual ~DataAccessorFactory() {}

        DataAccessorFactory(const DataAccessorFactory& other) = delete;
        DataAccessorFactory(DataAccessorFactory&& other) = delete;
        DataAccessorFactory& operator=(const DataAccessorFactory& other) = delete;
        DataAccessorFactory& operator=(DataAccessorFactory&& other) = delete;

      private:

        std::map<std::string, FactoryFnctType> factoriesMap_;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_FACTORY_HPP__
