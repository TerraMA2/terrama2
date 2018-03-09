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
 * \file interpolator/core/JSonUtils.hpp
 *
 * \brief Contains utility functions that can be used to turn json files into InterpolatorParams objects.
 *
 * \author Frederico Augusto Bedê
 */

#ifndef __TERRAMA2_SERVICES_INTERPOLATOR_CORE_JSONUTILS_HPP__
#define __TERRAMA2_SERVICES_INTERPOLATOR_CORE_JSONUTILS_HPP__

#include "../../../core/data-model/DataManager.hpp"

#include "Typedef.hpp"

// Qt
#include <QJsonObject>

namespace terrama2
{
  namespace services
  {
    namespace interpolator
    {
      namespace core
      {
        /*!
         * \brief Returns an InterpolatorParams object based on the \a json object.
         *
         * \param json Object that represents an .json file.
         *
         * \param dataManager The data manager being used.
         *
         * \return An InterpolatorParams object.
         */
        InterpolatorParamsPtr fromInterpolatorJson(QJsonObject json, terrama2::core::DataManager* dataManager);

        /*!
         * \brief Returns a QJasonObject object based on the \a interpolator object parameters.
         *
         * \param interpolator The object to be analysed.
         *
         * \return A QJsonObject that represents the paramaters object.
         */
        QJsonObject toJson(InterpolatorParamsPtr params);
      } // end namespace core
    }   // end namespace interpolator
  }     // end namespace services
}       // end namespace terrama2

#endif  // __TERRAMA2_SERVICES_INTERPOLATOR_CORE_JSONUTILS_HPP__
