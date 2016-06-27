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
  \file terrama2/services/maps/Shared.hpp

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_MAPS_SHARED_HPP__
#define __TERRAMA2_SERVICES_MAPS_SHARED_HPP__

#include <memory>

namespace terrama2
{
  namespace services
  {
    namespace maps
    {
      namespace core
      {
        struct Maps;
        //! Shared smart pointer for Maps
        typedef std::shared_ptr<const terrama2::services::maps::core::Maps> MapsPtr;

        class DataManager;
        //! Shared smart pointer for DataManager
        typedef std::shared_ptr<terrama2::services::maps::core::DataManager> DataManagerPtr;
      }
    }
  }
}


#endif // __TERRAMA2_SERVICES_MAPS_SHARED_HPP__
