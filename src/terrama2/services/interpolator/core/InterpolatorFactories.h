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
 * \file InterpolatorFactories.h
 *
 * \brief Defines factories for interpolators.
 *
 * \author Frederico Augusto Bedê
 */

#ifndef TERRAMA2_INTERPOLATOR_CORE_INTERPOLATORFACTORIES_H
#define TERRAMA2_INTERPOLATOR_CORE_INTERPOLATORFACTORIES_H

// TerraLib
#include <terralib/common/ParameterizedAbstractFactory.h>

namespace terrama2
{
  namespace services
  {
    namespace interpolator
    {
      namespace core
      {
        // Forward declarations
        struct Interpolator;
        struct InterpolatorParams;

        /*!
         * \class InterpolatorFactories
         *
         * \brief Implementation of the ParameterizedAbstractFactory to construct Interpolator objects given an InterpolatorParams instance.
         *
         * \tparam Interpolator Return type.
         *
         * \tparam int Type of the factories key.
         *
         * \tparam InterpolatorParams Parameters to be used to build the interpolators.
         *
         * \ingroup interpolator
         */
        class InterpolatorFactories : public te::common::ParameterizedAbstractFactory<Interpolator, int, const InterpolatorParams&>
        {
        public:

          /*!
           * \brief Constructor.
           *
           * \param key The identifier of the registered factory.
           */
          InterpolatorFactories(const int& key);

          /*!
           * \brief Initializes the interpolator factories. This method MUST BE called before the factories can be used.
           * It registers the specialized factories.
           */
          static void initialize();

          /*!
           * \brief Unregisters the interpolators factories. This method MUST BE called before the end of the service.
           */
          static void finalize();

        protected:

          static std::vector<InterpolatorFactories*> factories_;  //!< Registered factories.
        };

        /*!
         * \class NNInterpolatorFactory
         *
         * \brief Factory specialized to construct NNInterpolator objects. These objects realizes interpolation
         * using the nearest-neighbor algorithm.
         *
         * \ingroup interpolator
         */
        class NNInterpolatorFactory : public InterpolatorFactories
        {
        public:
          /*!
           * \brief Constructor.
           */
          NNInterpolatorFactory();

        protected:

          /*!
           * \brief build Constructs an NNInterpolator object.
           *
           * \param p The parameters to be used on build. It MUST BE an instance of NNInterpolatorParams, or it will not work well.
           *
           * \return A NNInterpolator object.
           */
          Interpolator* build(const InterpolatorParams& p);
        };

        /*!
         * \class BLInterpolatorFactory
         *
         * \brief Factory specialized to construct BLInterpolator objects. These objects realizes interpolation
         * using the bilinear algorithm.
         *
         * \ingroup interpolator
         */
        class BLInterpolatorFactory : public InterpolatorFactories
        {
        public:

          /*!
           * \brief Constructor.
           */
          BLInterpolatorFactory();

        protected:

          /*!
           * \brief build Constructs an BLInterpolator object.
           *
           * \param p The parameters to be used on build. It MUST BE an instance of BLInterpolatorParams, or it will not work well.
           *
           * \return A BLInterpolator object.
           */
          Interpolator* build(const InterpolatorParams& p);
        };

        /*!
         * \class BCInterpolatorFactory
         *
         * \brief Factory specialized to construct BCInterpolator objects. These objects realizes interpolation
         * using the bicubic algorithm.
         *
         * \ingroup interpolator
         */
        class BCInterpolatorFactory : public InterpolatorFactories
        {
        public:

          /*!
           * \brief Constructor.
           */
          BCInterpolatorFactory();

        protected:

          /*!
           * \brief build Constructs an BCInterpolator object.
           *
           * \param p The parameters to be used on build. It MUST BE an instance of BCInterpolatorParams, or it will not work well.
           *
           * \return A BCInterpolator object.
           */
          Interpolator* build(const InterpolatorParams& p);
        };
      } // end namespace core
    }   // end namespace interpolator
  }     // end namespace services
}       // end namespace terrama2

#endif // TERRAMA2_INTERPOLATOR_CORE_INTERPOLATORFACTORIES_H
