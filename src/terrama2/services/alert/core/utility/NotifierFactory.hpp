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
  \file terrama2/services/alert/core/utility/NotifierFactory.hpp

  \brief

  \author Jano Simas
*/

#ifndef __TERRAMA2_SERVICES_ALERT_CORE_NOTIFIER_FACTORY_HPP__
#define __TERRAMA2_SERVICES_ALERT_CORE_NOTIFIER_FACTORY_HPP__

#include "../Shared.hpp"

// TerraLib
#include <terralib/common/Singleton.h>

// STL
#include <functional>
#include <map>

namespace terrama2
{
  namespace services
  {
    namespace alert
    {
      namespace core
      {
        class NotifierFactory : public te::common::Singleton<NotifierFactory>
        {
          public:
            //! DataAccessor constructor function.
            typedef std::function<NotifierPtr (const std::map<std::string, std::string>& serverMap, ReportPtr report)> FactoryFnctType;
            //! Register a new DataAccessor constructor associated with the DataSeriesSemantics.
            void add(const std::string& notifierCode, FactoryFnctType f);
            //! Remove the DataAccessor constructor associated with the DataSeriesSemantics.
            void remove(const std::string& notifierCode);
            //! Check if there is a DataAccessor constructor associated with the DataSeriesSemantics.
            bool find(const std::string& notifierCode);
            /*!
              \brief Creates a DataAccessor

              The DataAccessor is constructed based on the DataSeriesSemantics of the DataSeries.

              \todo TODO: The DataAccessor will create a cache of the DataSeries data based on the Filter passed.

              \param dataProvider DataProvider of the dataSeries.
              \param dataSeries DataSeries of the data.
              \param filter Filtering information for caching data.
            */
            NotifierPtr make(const std::string& notifierCode, const std::map<std::string, std::string>& serverMap, ReportPtr report);

          protected:
            friend class te::common::Singleton<NotifierFactory>;

            //! Default constructor.
            NotifierFactory() = default;
            //! Default destructor
            virtual ~NotifierFactory() = default;

            NotifierFactory(const NotifierFactory& other) = delete;
            NotifierFactory(NotifierFactory&& other) = delete;
            NotifierFactory& operator=(const NotifierFactory& other) = delete;
            NotifierFactory& operator=(NotifierFactory&& other) = delete;

          private:

            std::map<std::string, FactoryFnctType> factoriesMap_;
        };
      } /* core */
    } /* alert */
  } /* services */
} /* terrama2 */

#endif //__TERRAMA2_SERVICES_ALERT_CORE_NOTIFIER_FACTORY_HPP__
