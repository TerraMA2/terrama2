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
  \file terrama2/core/utility/DataAccessFactory.hpp

  \brief

  \author Jano Simas
 */

 #ifndef __TERRAMA2_CORE_UTILITY_DATA_ACCESSOR_FACTORY_HPP__
 #define __TERRAMA2_CORE_UTILITY_DATA_ACCESSOR_FACTORY_HPP__

#include "../Shared.hpp"
#include "../data-model/Filter.hpp"

// TerraLib
#include <terralib/common/Singleton.h>

namespace terrama2
{
  namespace core
  {
    class DataAccessFactory : public te::common::Singleton<DataAccessFactory>
    {
    public:

      DataAccessorPtr makeDataAccessor(DataProviderPtr dataProvider, DataSeriesPtr dataSeries, Filter filter = Filter());

    protected:
      friend class te::common::Singleton<DataAccessFactory>;

      DataAccessFactory() {}
      ~DataAccessFactory() {}

      DataAccessFactory(const DataAccessFactory& other) = delete;
      DataAccessFactory(DataAccessFactory&& other) = delete;
      DataAccessFactory& operator=(const DataAccessFactory& other) = delete;
      DataAccessFactory& operator=(DataAccessFactory&& other) = delete;
    };


  } /* core */

} /* terrama2 */

#endif // __TERRAMA2_CORE_UTILITY_DATA_ACCESSOR_FACTORY_HPP__
