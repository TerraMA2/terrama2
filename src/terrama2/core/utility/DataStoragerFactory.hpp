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
  \file terrama2/core/utility/DataStoragerFactory.hpp

  \brief

  \author Jano Simas
 */

 #ifndef __TERRAMA2_CORE_UTILITY_DATA_STORAGER_FACTORY_HPP__
 #define __TERRAMA2_CORE_UTILITY_DATA_STORAGER_FACTORY_HPP__

#include "../Shared.hpp"
#include "../data-model/DataProvider.hpp"

// TerraLib
#include <terralib/common/Singleton.h>

// STL
#include <functional>
#include <map>

namespace terrama2
{
  namespace core
  {
    class DataStoragerFactory : public te::common::Singleton<DataStoragerFactory>
    {
    public:

      typedef std::function<DataStorager* (terrama2::core::DataProviderPtr dataProvider)> FactoryFnctType;

      void add(const terrama2::core::DataProviderType& dataProviderType, FactoryFnctType f);

      void remove(const terrama2::core::DataProviderType& dataProviderType);

      bool find(const terrama2::core::DataProviderType& dataProviderType);

      terrama2::core::DataStoragerPtr make(terrama2::core::DataProviderPtr dataProvider) const;


    protected:
      friend class te::common::Singleton<DataStoragerFactory>;

      DataStoragerFactory() {}
      ~DataStoragerFactory() {}

      DataStoragerFactory(const DataStoragerFactory& other) = delete;
      DataStoragerFactory(DataStoragerFactory&& other) = delete;
      DataStoragerFactory& operator=(const DataStoragerFactory& other) = delete;
      DataStoragerFactory& operator=(DataStoragerFactory&& other) = delete;

      std::map<terrama2::core::DataProviderType, FactoryFnctType> factoriesMap_;
    };


  } /* core */

} /* terrama2 */

#endif // __TERRAMA2_CORE_UTILITY_DATA_STORAGER_FACTORY_HPP__
