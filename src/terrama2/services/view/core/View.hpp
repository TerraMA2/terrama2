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
  \file terrama2/services/view/core/View.hpp

  \brief Model class for the view configuration.

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_VIEW_CORE_VIEW_HPP__
#define __TERRAMA2_SERVICES_VIEW_CORE_VIEW_HPP__

// TerraMA2
#include "../../../core/data-model/Process.hpp"
#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-access/DataSetSeries.hpp"
#include "../../../core/data-model/Schedule.hpp"
#include "../../../core/data-model/Filter.hpp"
#include "../../../core/utility/Logger.hpp"
#include "../../../core/Shared.hpp"
#include "../../../core/Typedef.hpp"
#include "Typedef.hpp"
#include "Shared.hpp"
#include "ViewLogger.hpp"
#include "Exception.hpp"

#include <terralib/maptools/Enums.h>

// STL
#include <string>
#include <vector>
#include <map>

namespace terrama2
{
  namespace services
  {
    namespace view
    {
      namespace core
      {
        namespace ViewTags
        {
          const std::string WORKSPACE = "workspace";
          const std::string LAYER_NAME = "layer_name";
          const std::string LAYER_TITLE = "layer_title";
        } /* ViewTags */

        /*!
          \brief The View groups the information to draw view.
        */
        struct View : public terrama2::core::Process
        {
          struct Legend
          {
              enum class ObjectType
              {
                UNKNOWN = 1,
                RASTER = 2,
                GEOMETRY = 3
              };

              enum class CREATION_TYPE
              {
                EDITOR = 0,
                XML = 1
              };

              enum class OperationType
              {
                EQUAL_STEPS = 1,
                QUANTIL = 2,
                VALUE = 3
              };

              enum class ClassifyType
              {
                RAMP = 1,
                INTERVALS = 2,
                VALUES = 3
              };

              struct Rule
              {
                  std::string title = "";
                  std::string value = "";
                  std::string color = "";
                  std::string opacity = "1";
                  bool isDefault = false;

                  bool operator ==(const Rule& other) const
                  {
                    return (title == other.title &&
                            value == other.value &&
                            color == other.color &&
                            opacity == other.opacity &&
                            isDefault == other.isDefault);
                  }

                  static bool compareByNumericValue(const Rule& a,
                                                    const Rule& b)
                  {
                    if(a.isDefault)
                      return true;

                    if(b.isDefault)
                      return false;

                    try
                    {
                      auto x = std::stold(a.value);
                      auto y = std::stold(b.value);

                      return x < y;
                    }
                    catch(const std::invalid_argument& e)
                    {
                      TERRAMA2_LOG_ERROR() << "Invalid value for legend: " << e.what();
                      return false;
                    }
                  }
              };

              /*!
               * \brief Retrieves string representation of classify type supported by GeoServer
               *
               * \param classify - Type of ColorMap handling
               * \return String representation of GeoServer ColorMap Classify Type
               */
              static std::string to_string(ClassifyType classify)
              {
                switch(classify)
                {
                  case ClassifyType::INTERVALS:
                    return "intervals";
                  case ClassifyType::RAMP:
                    return "ramp";
                  case ClassifyType::VALUES:
                    return "values";
                  default:
                    throw Exception() << ErrorDescription("Invalid View Classification type");
                }
              }

              OperationType operation = OperationType::VALUE;
              ClassifyType classify;
              std::unordered_map<std::string, std::string> metadata;
              std::vector< Rule > rules;
          };

          std::string viewName = "";

          DataSeriesId dataSeriesID; //!< DataSeries ID that compose this view
          terrama2::core::Filter filter; //!< Filter
          std::unique_ptr<Legend> legend;

          // Parameters to generate a image
          std::string imageName = "";
          te::map::ImageType imageType = te::map::ImageType(1);
          uint32_t imageResolutionWidth = 0; //!< Width resolution of view in pixels
          uint32_t imageResolutionHeight = 0; //!< Height resolution of view in pixels

          uint32_t srid = 0; //!< SRID to aplly in view

          std::map<std::string, std::string> properties;//! Completementary information of the view.
        };

      } // end namespace core
    }   // end namespace view
  }     // end namespace services
} // end namespace terrama2

#endif //__TERRAMA2_SERVICES_VIEW_CORE_VIEW_HPP__
