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
  \file terrama2/core/utility/DataRetrieverFactory.hpp

  \brief

  \author Jano Simas
 */

#ifndef __TERRAMA2_CORE_UTILITY_DATA_RETRIEVER_FACTORY_HPP__
#define __TERRAMA2_CORE_UTILITY_DATA_RETRIEVER_FACTORY_HPP__

// TerraLib
#include <terralib/common/Singleton.h>
// STL
#include <functional>
#include <map>

// TerraMa2
#include "../Config.hpp"
#include "../Shared.hpp"
#include "../Shared.hpp"
#include "../Typedef.hpp"
#include "../data-model/DataProvider.hpp"

namespace terrama2
{
  namespace core
  {
    class TMCOREEXPORT DataRetrieverFactory : public te::common::Singleton<DataRetrieverFactory>
    {
      public:
        //! DataRetriever constructor function.
        typedef std::function<DataRetrieverPtr (terrama2::core::DataProviderPtr dataProvider)> FactoryFnctType;
        //! Register a new DataRetriever constructor associated with the DataProviderType.
        void add(const DataProviderType& dataProviderType, FactoryFnctType f);
        //! Remove the DataRetriever constructor associated with the DataProviderType.
        void remove(const DataProviderType& dataProviderType);
        //PAULO: documentar!
        bool find(const DataProviderType& dataProviderType);
        /*!
          \brief Creates a DataRetriever

          The DataRetriever is constructed based on the DataProviderType of the DataProvider.
        */
        terrama2::core::DataRetrieverPtr make(terrama2::core::DataProviderPtr dataProvider) const;


      protected:
        friend class te::common::Singleton<DataRetrieverFactory>;
        //! Default constructor
        DataRetrieverFactory() {}
        //! Default destructor
        ~DataRetrieverFactory() {}

        DataRetrieverFactory(const DataRetrieverFactory& other) = delete;
        DataRetrieverFactory(DataRetrieverFactory&& other) = delete;
        DataRetrieverFactory& operator=(const DataRetrieverFactory& other) = delete;
        DataRetrieverFactory& operator=(DataRetrieverFactory&& other) = delete;

        std::map<DataProviderType, FactoryFnctType> factoriesMap_;
    };


  } /* core */

} /* terrama2 */

#endif // __TERRAMA2_CORE_UTILITY_DATA_RETRIEVER_FACTORY_HPP__
