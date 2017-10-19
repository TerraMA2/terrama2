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
 * \file Interpolator.hpp
 *
 * \brief This class defines an interpolator.
 *
 * \author Frederico Augusto Bedê
 *
 * \defgroup interpolator Interpolator framework.
 * Defines classes and structures for executing algorithms of interpolation over the PCDs data from TerraMA2.
 */

#ifndef __TERRAMA2_SERVICES_INTERPOLATOR_CORE_INTERPOLATOR_HPP__
#define __TERRAMA2_SERVICES_INTERPOLATOR_CORE_INTERPOLATOR_HPP__

#include "../../../core/data-model/Process.hpp"
#include "../../../core/Typedef.hpp"

#include "InterpolatorParams.hpp"
#include "Typedef.hpp"

// TerraLib
#include <terralib/raster/Raster.h>

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
        /*!
         * \struct Interpolator
         *
         * \brief Generic structure for an interpolator object.
         *
         * There are three algorithms to use on interpolations:
         *
         * <UL>
         *  <LI>Nearest-neighbor interpolation.</LI>
         *  <LI>Simple average of the nearest neighbor interpolation.</LI>
         *  <LI>Weight average of the nearest neighbor interpolation (inverse of square distance).</LI>
         * </UL>
         *
         * Youd can be found more information about interpolation methods in: <A HREF="http://www.gisresources.com/types-interpolation-methods_3/">Interpolation methods on GIS Resources main site.</A>
         *
         * \ingroup interpolator
         */
        struct Interpolator : public terrama2::core::Process
        {
          /*!
           * \brief Default constructor.
           *
           * \param params The parameters to be used.
           */
          Interpolator(InterpolatorParamsPtr params);

          /*!
           * \brief Virtual destructor.
           */
          virtual ~Interpolator();

          /*!
           * \brief Method must be implemented by the subclasses to execute the specific algorithm.
           *
           * \return The interpolated raster.
           *
           * \exception If the bounding rect, of the parameters used by the interpolator, is invalid a NoDataException will be raised.
           */
          virtual te::rst::RasterPtr makeInterpolation() = 0;

        protected:

          /*!
           * \brief Builds a raster with the given parameters.
           *
           * \return A raster with the dimensions defined by the parameters.
           *
           * \exception If the bounding rect, of the parameters used by the interpolator, is invalid a NoDataException will be raised.
           */
          te::rst::RasterPtr makeRaster();

          /*!
           * \brief Fills the kd-tree with the data defined by the parameters. This is usefull to quickly find the neighbors used in the
           * computations.
           *
           * \exception If the bounding rect, of the parameters used by the interpolator, is invalid a NoDataException will be raised.
           */
          void fillTree();

          std::shared_ptr<InterpolatorParams> interpolationParams_; //!< Parameters of interpolation.

          std::unique_ptr<InterpolatorTree> tree_;    //!< A kd-tree used to determine neighborhood.
        };

        /*!
         * \struct NNInterpolator
         *
         * \brief Interpolator specialized to execute the nearest-neighbor algorithm of interpolation.
         *
         * \ingroup interpolator
         */
        struct NNInterpolator : public Interpolator
        {
          /*!
           * \brief Constructor.
           *
           * \param params The parameters to used to build an object of this type.
           *
           * \note \a params MUST be an instance of NNInterpolatorParams structure, or it will not work properly.
           */
          NNInterpolator(InterpolatorParamsPtr params);

          /*!
           * \brief Executes interpolation based on the nearest-neighbor approach.
           *
           * \return The interpolated raster.
           */
          te::rst::RasterPtr makeInterpolation();
        };

        /*!
         * \struct AvgDistInterpolator
         *
         * \brief Interpolator specialized to execute simple average of the neighbors algorithm of interpolation.
         *
         * \ingroup interpolator
         */
        struct AvgDistInterpolator : public Interpolator
        {
          /*!
           * \brief Constructor.
           *
           * \param params The parameters to used to build an object of this type.
           *
           * \note \a params MUST be an instance of AvgDistInterpolatorParams structure, or it will not work properly.
           */
          AvgDistInterpolator(InterpolatorParamsPtr params);

          /*!
           * \brief Executes interpolation based on the simple average neighbor approach.
           *
           * \return The interpolated raster.
           */
          te::rst::RasterPtr makeInterpolation();
        };

        /*!
         * \struct SqrAvgDistInterpolator
         *
         * \brief Interpolator specialized to execute the weight average neighbor algorithm of interpolation.
         *
         * \ingroup interpolator
         */
        struct SqrAvgDistInterpolator : public Interpolator
        {
          /*!
           * \brief Constructor.
           *
           * \param params The parameters to used to build an object of this type.
           *
           * \note \a params MUST be an instance of SqrAvgDistInterpolatorParams structure, or it will not work properly.
           */
          SqrAvgDistInterpolator(InterpolatorParamsPtr params);

          /*!
           * \brief Executes interpolation based on the bilinear approach.
           *
           * \return The interpolated raster.
           */
          te::rst::RasterPtr makeInterpolation();
        };
      } // end namespace core
    }   // end namespace interpolator
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_SERVICES_INTERPOLATOR_CORE_INTERPOLATOR_HPP__
