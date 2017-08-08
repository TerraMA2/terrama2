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
  \file interpolator/core/Service.hpp

  \brief This class defines an interpolator.

  \author Frederico Augusto Bedê
*/

#ifndef __TERRAMA2_SERVICES_INTERPOLATOR_CORE_SERVICE_HPP__
#define __TERRAMA2_SERVICES_INTERPOLATOR_CORE_SERVICE_HPP__

//#include "../../../core/utility/Service.hpp"
//#include "../../../core/Typedef.hpp"
//#include "../../../core/Shared.hpp"
//#include "../core/Shared.hpp"
//#include "../core/Typedef.hpp"
//#include "DataManager.hpp"
//#include "CollectorLogger.hpp"

// STL
#include <memory>

namespace terrama2
{
  namespace services
  {
    namespace interpolator
    {
      namespace core
      {
        struct Interpolator : public terrama2::core::Process
        {

        };
      } // end namespace core
    }   // end namespace interpolator
  }     // end namespace services
} // end namespace terrama2

#endif //__TERRAMA2_SERVICES_INTERPOLATOR_CORE_SERVICE_HPP__
