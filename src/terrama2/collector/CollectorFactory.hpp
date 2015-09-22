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
  \file terrama2/collector/CollectorFactory.hpp

  \brief Instantiate collectors for DataProviders.

  \author Jano Simas
*/


#ifndef __TERRAMA2_COLLECTOR_COLLECTORFACTORY_HPP__
#define __TERRAMA2_COLLECTOR_COLLECTORFACTORY_HPP__

#include "Collector.hpp"

//Qt
#include <QMap>

//Teralib
#include <terralib/common/Singleton.h>

namespace terrama2
{
  namespace core {
    class DataSet;
    class Data;
  }
  namespace collector
  {

    /*!
         * \brief The CollectorFactory class is responsible for creating the appropriate type of Collector.
         *
         *       The CollectorFactory is a singleton responsible for creating the appropriate type of Collector.
         *
         * The method CollectorFactory::getCollector will use the DataProvider information to
         * create an instace of a collector of the appropriate derived classe and return a shared pointer to it.
         *
         */
    class CollectorFactory : public te::common::Singleton<CollectorFactory>
    {
      public:

        /*!
             * \brief Returns the instace of the collector or instatiate a new collector of the appropriate derived classe and return a shared pointer to it.
             * \param dataProvider Data provider information.
             * \return Shared pointer to the new collector.
             *
             * \exception terrama2::collector::UnknownDataProviderKindException Raised when CollectorFactory cannot identify the right Collector type for the DataProvider.
             */
        CollectorPtr getCollector(const core::DataProviderPtr dataProvider);

        /*!
         * \brief Remove the collector from the list.
         *
         * If there is no collector for the dataProvider, nothing is done.
         *
         * \param dataProvider Collector's DataProvider.
         */
        void removeCollector(const core::DataProviderPtr dataProvider);

      private:

        QMap<int /*DataProviderId*/, CollectorPtr> collectorMap_;

    };
  }
}


#endif //__TERRAMA2_COLLECTOR_COLLECTORFACTORY_HPP__
