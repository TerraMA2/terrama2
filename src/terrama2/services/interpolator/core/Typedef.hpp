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
  \file interpolator/core/Typedef.hpp

  \brief Contains definitions of types used by the interpolation mechanism.

  \author Frederico Augusto Bedê
*/

#ifndef __TERRAMA2_SERVICES_INTERPOLATOR_TYPEDEF_HPP__
#define __TERRAMA2_SERVICES_INTERPOLATOR_TYPEDEF_HPP__

#include "../../../core/data-access/DataSetSeries.hpp"
#include "../../../core/data-model/Process.hpp"
//#include "../../../core/Typedef.hpp"

// TerraLib
#include <terralib/geometry/Point.h>
#include <terralib/sam/kdtree.h>


//! Unique identifier of a Interpolator
typedef ProcessId InterpolatorId;

namespace te
{
  namespace rst
  {
    // Forward declarations
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
        // Forward declaration
        class DataManager;
        struct Interpolator;
        struct InterpolatorParams;

        //! Node of a kd-tree specialized to use with the Interpolator.
        typedef te::sam::kdtree::Node<te::gm::Point, terrama2::core::DataSetSeries, terrama2::core::DataSetSeries> InterpolatorNode;

        //! Kd-tree specialized to use InterpolatorNode.
        typedef te::sam::kdtree::Index<InterpolatorNode> InterpolatorTree;

        //! Shared smart pointer for InterpolatorParams
        typedef std::shared_ptr<terrama2::services::interpolator::core::InterpolatorParams> InterpolatorParamsPtr;

        //! Shared smart pointer for Interpolator
        typedef std::shared_ptr<Interpolator> InterpolatorPtr;

        //! Shared smart pointer for DataManager
        typedef std::shared_ptr<terrama2::services::interpolator::core::DataManager> DataManagerPtr;

        //! Shared smart pointer for te::rst::Raster
        typedef std::shared_ptr<te::rst::Raster> RasterPtr;
      }
    }
  }
}

#endif // __TERRAMA2_SERVICES_INTERPOLATOR_TYPEDEF_HPP__
