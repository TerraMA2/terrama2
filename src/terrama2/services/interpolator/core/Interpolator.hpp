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
 * \file interpolator/core/Service.hpp
 *
 * \brief This class defines an interpolator.
 *
 * \author Frederico Augusto Bedê
 *
 * \todo Finish the comments of the file interpolator/core/Service.hpp.
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
         * \struct
         *
         * \brief The Interpolator struct
         */
        struct Interpolator : public terrama2::core::Process
        {
          /*!
           * \brief
           *
           * \param
           */
          Interpolator(InterpolatorParamsPtr params);

          /*!
           * \brief makeInterpolation
           *
           * \return
           */
          virtual RasterPtr makeInterpolation() = 0;

          /*!
           * \brief
           */
          void fillTree();

          InterpolatorParamsPtr interpolationParams_; //!< Parameters of interpolation.

          std::unique_ptr<InterpolatorTree> tree_;    //!<
        };

        /*!
         * \struct
         *
         * \brief The NNInterpolator struct
         */
        struct NNInterpolator : public Interpolator
        {
          /*!
           * \brief
           *
           * \param
           */
          NNInterpolator(InterpolatorParamsPtr params);

          /*!
           * \brief makeInterpolation
           * \return
           */
          RasterPtr makeInterpolation();
        };

        /*!
         * \struct
         *
         * \brief The BLInterpolator struct
         */
        struct BLInterpolator : public Interpolator
        {
          /*!
           * \brief
           *
           * \param
           */
          BLInterpolator(InterpolatorParamsPtr params);

          /*!
           * \brief makeInterpolation
           * \return
           */
          RasterPtr makeInterpolation();
        };

        /*!
         * \struct
         *
         * \brief The BCInterpolator struct
         */
        struct BCInterpolator : public Interpolator
        {
          /*!
           * \brief BCInterpolator
           * \param params
           */
          BCInterpolator(InterpolatorParamsPtr params);

          /*!
           * \brief makeInterpolation
           * \return
           */
          RasterPtr makeInterpolation();
        };
      } // end namespace core
    }   // end namespace interpolator
  }     // end namespace services
} // end namespace terrama2

#endif //__TERRAMA2_SERVICES_INTERPOLATOR_CORE_INTERPOLATOR_HPP__
