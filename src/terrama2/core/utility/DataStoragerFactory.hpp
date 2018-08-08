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


// TerraLib
#include <terralib/common/Singleton.h>
// STL
#include <functional>
#include <map>
#include <string>

// TerraMa2
#include "../Config.hpp"
#include "../Shared.hpp"
#include "../Typedef.hpp"
#include "../data-model/DataProvider.hpp"

namespace terrama2
{
  namespace core
  {
    class TMCOREEXPORT DataStoragerFactory : public te::common::Singleton<DataStoragerFactory>
    {
      public:
        //! DataStorager constructor function.
        typedef std::function<DataStoragerPtr(terrama2::core::DataSeriesPtr dataSeries, terrama2::core::DataProviderPtr dataProvider)> FactoryFnctType;
        //! Register a new DataStorager constructor associated with the DataProviderType.
        void add(const std::string& code, FactoryFnctType f);
        //! Remove the DataStorager constructor associated with the DataProviderType.
        void remove(const std::string& code);
        //! Returns if exists a factory for the given format.
        bool find(const std::string& code);
        /*!
          \brief Creates a DataStorager

          The DataStorager is constructed based on the format of the DataSeries.
        */
        terrama2::core::DataStoragerPtr make(terrama2::core::DataSeriesPtr outputDataSeries, terrama2::core::DataProviderPtr dataProvider) const;


      protected:
        friend class te::common::Singleton<DataStoragerFactory>;
        //! Default constructor
        DataStoragerFactory() {}
        //! Default destructor
        ~DataStoragerFactory() {}

        DataStoragerFactory(const DataStoragerFactory& other) = delete;
        DataStoragerFactory(DataStoragerFactory&& other) = delete;
        DataStoragerFactory& operator=(const DataStoragerFactory& other) = delete;
        DataStoragerFactory& operator=(DataStoragerFactory&& other) = delete;

        std::map<DataFormat, FactoryFnctType> factoriesMap_;
    };


  } /* core */

} /* terrama2 */

#endif // __TERRAMA2_CORE_UTILITY_DATA_STORAGER_FACTORY_HPP__
