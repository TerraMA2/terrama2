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
  \file InterpolatorFactories.h

  \brief

  \author Frederico Augusto Bedê
*/

#ifndef TERRAMA2_INTERPOLATOR_CORE_INTERPOLATORFACTORIES_H
#define TERRAMA2_INTERPOLATOR_CORE_INTERPOLATORFACTORIES_H

//#include "Shared.hpp"
//#include "Typedef.hpp"

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
         * \class
         *
         * \brief The InterpolatorFactories class
         */
        class InterpolatorFactories : public te::common::ParameterizedAbstractFactory<Interpolator, int, InterpolatorParams>
        {
        public:

          /*!
           * \brief InterpolatorFactories
           * \param key
           */
          InterpolatorFactories(const int& key);
        };

        /*!
         * \class
         *
         * \brief The NNInterpolatorFactory class
         */
        class NNInterpolatorFactory : public InterpolatorFactories
        {
        public:
          /*!
           * \brief NNInterpolatorFactory
           */
          NNInterpolatorFactory();

        protected:

          /*!
           * \brief build
           * \param p
           * \return
           */
          Interpolator* build(InterpolatorParams p);
        };

        /*!
         * \class
         *
         * \brief The BLInterpolatorFactory class
         */
        class BLInterpolatorFactory : public InterpolatorFactories
        {
        public:

          /*!
           * \brief BLInterpolatorFactory
           */
          BLInterpolatorFactory();

        protected:

          /*!
           * \brief build
           * \param p
           * \return
           */
          Interpolator* build(InterpolatorParams p);
        };

        /*!
         * \class
         *
         * \brief The BCInterpolatorFactory class
         */
        class BCInterpolatorFactory : public InterpolatorFactories
        {
        public:

          /*!
           * \brief BCInterpolatorFactory
           */
          BCInterpolatorFactory();

        protected:

          /*!
           * \brief build
           * \param p
           * \return
           */
          Interpolator* build(InterpolatorParams p);
        };
      } // namespace core
    } // namespace interpolator
  } //services
} // namespace terrama2

#endif // TERRAMA2_INTERPOLATOR_CORE_INTERPOLATORFACTORIES_H
