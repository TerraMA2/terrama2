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
 *
 * \todo Implements the interpolator based on bicubic algorithm.
 * \todo Implements the interpolator based on bicubic algorithm.
 */

#ifndef __TERRAMA2_SERVICES_INTERPOLATOR_CORE_INTERPOLATOR_HPP__
#define __TERRAMA2_SERVICES_INTERPOLATOR_CORE_INTERPOLATOR_HPP__

#include "../../../core/data-model/Process.hpp"
#include "../../../core/Typedef.hpp"

#include "InterpolatorParams.hpp"
#include "Typedef.hpp"

// STL
#include <memory>

namespace te
{
  namespace rst
  {
    // Forward declaration
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
        /*!
         * \struct Interpolator
         *
         * \brief Generic structure for an interpolator object.
         *
         * There are three algorithms to use on interpolations:
         *
         * <UL>
         *  <LI>Nearest-neighbor interpolation.</LI>
         *  <LI>Bilinerar interpolation.</LI>
         *  <LI>Bicubic interpolation.</LI>
         * </UL>
         *
         * The aproach of the nearest neighbor aproach uses the nearest sample value to use as value of some coordinate being analysed. For more details
         * about this aproach can be found in: <A HREF="https://en.wikipedia.org/wiki/Nearest-neighbor_interpolation">Nearest-neighbor interpolation on Wikipedia.</A>
         *
         * The aproach of the bilinear interpolation uses the bilinear algorithm to interpolate. For more details
         * about this aproach can be found in: <A HREF="https://en.wikipedia.org/wiki/Bilinear_interpolation">Bilinear interpolation on Wikipedia.</A>
         *
         * The aproach of the bicubic interpolation uses the bicubic algorithm to interpolate. For more details
         * about this aproach can be found in: <A HREF="https://en.wikipedia.org/wiki/Bicubic_interpolation">Bicubic interpolation on Wikipedia.</A>
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
           * \brief Method must be implemented by the subclasses to execute the specific algorithm.
           *
           * \return The interpolated raster.
           */
          virtual RasterPtr makeInterpolation() = 0;

          /*!
           * \brief Fills the kd-tree with the data defined by the parameters. This is usefull to quickly find the neighbors used in the
           * computations.
           */
          void fillTree();

          InterpolatorParamsPtr interpolationParams_; //!< Parameters of interpolation.

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
           * \note \a params MUST be an instance of NNInterpolatorParams structure, or it will not work well.
           */
          NNInterpolator(InterpolatorParamsPtr params);

          /*!
           * \brief Executes interpolation based on the nearest-neighbor approach.
           *
           * \return The interpolated raster.
           */
          RasterPtr makeInterpolation();
        };

        /*!
         * \struct BLInterpolator
         *
         * \brief Interpolator specialized to execute the bilinear algorithm of interpolation.
         *
         * \ingroup interpolator
         */
        struct BLInterpolator : public Interpolator
        {
          /*!
           * \brief Constructor.
           *
           * \param params The parameters to used to build an object of this type.
           *
           * \note \a params MUST be an instance of BLInterpolatorParams structure, or it will not work well.
           */
          BLInterpolator(InterpolatorParamsPtr params);

          /*!
           * \brief Executes interpolation based on the bilinear approach.
           *
           * \return The interpolated raster.
           */
          RasterPtr makeInterpolation();
        };

        /*!
         * \struct BCInterpolator
         *
         * \brief Interpolator specialized to execute the bicubic algorithm of interpolation.
         *
         * \ingroup interpolator
         */
        struct BCInterpolator : public Interpolator
        {
          /*!
           * \brief Constructor.
           *
           * \param params The parameters to used to build an object of this type.
           *
           * \note \a params MUST be an instance of BCInterpolatorParams structure, or it will not work well.
           */
          BCInterpolator(InterpolatorParamsPtr params);

          /*!
           * \brief Executes interpolation based on the bilinear approach.
           *
           * \return The interpolated raster.
           */
          RasterPtr makeInterpolation();
        };
      } // end namespace core
    }   // end namespace interpolator
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_SERVICES_INTERPOLATOR_CORE_INTERPOLATOR_HPP__
