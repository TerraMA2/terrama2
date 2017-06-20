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
#include "../data-model/DataSeriesSemantics.hpp"

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
      \brief DataAccessorFactory allows the creation of a DataAcessor.

    */
    class DataAccessorFactory : public te::common::Singleton<DataAccessorFactory>
    {
      public:
        //! DataAccessor constructor function.
        typedef std::function<DataAccessorPtr (terrama2::core::DataProviderPtr dataProvider, terrama2::core::DataSeriesPtr dataSeries)> FactoryFnctType;
        //! Register a new DataAccessor constructor associated with the DataSeriesSemantics.
        void add(const SemanticsDriver& semanticsDriver, FactoryFnctType f);
        //! Remove the DataAccessor constructor associated with the DataSeriesSemantics.
        void remove(const SemanticsDriver& semanticsDriver);
        //! Check if there is a DataAccessor constructor associated with the DataSeriesSemantics.
        bool find(const SemanticsDriver& semanticsDriver);
        /*!
          \brief Creates a DataAccessor

          The DataAccessor is constructed based on the DataSeriesSemantics of the DataSeries.

          \todo TODO: The DataAccessor will create a cache of the DataSeries data based on the Filter passed.

          \param dataProvider DataProvider of the dataSeries.
          \param dataSeries DataSeries of the data.
          \param filter Filtering information for caching data.
        */
        terrama2::core::DataAccessorPtr make(terrama2::core::DataProviderPtr dataProvider, terrama2::core::DataSeriesPtr dataSeries);

      protected:
        friend class te::common::Singleton<DataAccessorFactory>;

        //! Default constructor.
        DataAccessorFactory() = default;
        //! Default destructor
        virtual ~DataAccessorFactory() = default;

        DataAccessorFactory(const DataAccessorFactory& other) = delete;
        DataAccessorFactory(DataAccessorFactory&& other) = delete;
        DataAccessorFactory& operator=(const DataAccessorFactory& other) = delete;
        DataAccessorFactory& operator=(DataAccessorFactory&& other) = delete;

      private:

        std::map<SemanticsDriver, FactoryFnctType> factoriesMap_;
    };
  }
}

#endif // __TERRAMA2_CORE_DATA_ACCESS_DATA_ACCESSOR_FACTORY_HPP__
