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
  \file interpolator/core/Shared.hpp

  \author Frederico Augusto Bedê
*/

#ifndef __TERRAMA2_SERVICES_INTERPOLATOR_SHARED_HPP__
#define __TERRAMA2_SERVICES_INTERPOLATOR_SHARED_HPP__

// STL
#include <memory>

namespace te
{
  namespace rst
  {
    class Raster;
  }
}

namespace terrama2
{
  namespace services
  {
    namespace interpolator
    {
      namespace core
      {
        // Forward declarations
        class DataManager;
        struct Interpolator;
        struct InterpolatorParams;

        //! Shared smart pointer for Interpolator
        typedef std::shared_ptr<terrama2::services::interpolator::core::Interpolator> InterpolatorPtr;

        //! Shared smart pointer for InterpolatorParams
        typedef std::shared_ptr<terrama2::services::interpolator::core::InterpolatorParams> InterpolatorParamsPtr;

        //! Shared smart pointer for DataManager
        typedef std::shared_ptr<terrama2::services::interpolator::core::DataManager> DataManagerPtr;

        //! Shared smart pointer for te::rst::Raster
        typedef std::shared_ptr<te::rst::Raster> RasterPtr;
      } /* core */
    }   /* interpolator */
  }     /* services */
}       /* terrama2 */

#endif // __TERRAMA2_SERVICES_INTERPOLATOR_SHARED_HPP__
