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
  \file terrama2/services/view/core/MapsServerFactory.hpp

  \brief

  \author Vinicius Campanha
 */

#ifndef __TERRAMA2_SERVICES_VIEW_CORE_MAPS_SERVER_FACTORY_HPP__
#define __TERRAMA2_SERVICES_VIEW_CORE_MAPS_SERVER_FACTORY_HPP__

// TerraMA2
#include "Typedef.hpp"
#include "Shared.hpp"

// TerraLib
#include <terralib/common/Singleton.h>
#include <terralib/core/uri/URI.h>

// STL
#include <functional>
#include <map>

namespace terrama2
{
  namespace services
  {
    namespace view
    {
      namespace core
      {
        class MapsServerFactory : public te::common::Singleton<MapsServerFactory>
        {
          public:
            //! MapsServer constructor function.
            typedef std::function< MapsServerPtr (te::core::URI uri)> FactoryFnctType;
            //! Register a new maps server constructor associated with the MapsServerType.
            void add(const MapsServerType& mapsServertype, FactoryFnctType f);
            //! Remove the maps server constructor associated with the MapsServerType.
            void remove(const MapsServerType& mapsServertype);
            //! Check if the maps server is registered
            bool find(const MapsServerType& mapsServertype);

            /*!
            \brief Creates a maps server

            The maps server is constructed based on the MapsServerType.
            */
            MapsServerPtr make(const te::core::URI uri,
                               const MapsServerType& mapsServertype) const;


          protected:
            friend class te::common::Singleton<MapsServerFactory>;
            //! Default constructor
            MapsServerFactory() {}
            //! Default destructor
            ~MapsServerFactory() {}

            MapsServerFactory(const MapsServerFactory& other) = delete;
            MapsServerFactory(MapsServerFactory&& other) = delete;
            MapsServerFactory& operator=(const MapsServerFactory& other) = delete;
            MapsServerFactory& operator=(MapsServerFactory&& other) = delete;

            std::map<MapsServerType, FactoryFnctType> factoriesMap_;
        };


      } /* core */
    }
  }

} /* terrama2 */

#endif // __TERRAMA2_SERVICES_VIEW_CORE_MAPS_SERVER_FACTORY_HPP__
