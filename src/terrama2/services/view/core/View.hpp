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


// STL
#include <string>
#include <vector>

// TerraLib
#include <terralib/se/Style.h>
#include <terralib/core/uri/URI.h>

// TerraMA2
#include "../../../core/data-model/Process.hpp"
#include "../../../core/data-model/DataSeries.hpp"
#include "../../../core/data-access/DataSetSeries.hpp"
#include "../../../core/data-model/Schedule.hpp"
#include "../../../core/data-model/Filter.hpp"
#include "../../../core/Shared.hpp"
#include "../../../core/Typedef.hpp"
#include "MemoryDataSetLayer.hpp"
#include "Typedef.hpp"
#include "Shared.hpp"
#include "ViewLogger.hpp"

namespace terrama2
{
  namespace services
  {
    namespace view
    {
      namespace core
      {
        /*!
          \brief The View groups the information to draw view.
        */
        struct View : public terrama2::core::Process
        {
            struct Legend
            {
                enum OperationType
                {
                  EQUAL_STEPS = 1,
                  QUANTIL = 2,
                  VALUE = 3
                };

                struct Rule
                {
                    std::string title = "";
                    std::string value = "";
                    std::string color = "";
                    bool isDefault = false;
                };

                OperationType ruleType;
                int band_number = 0;
                std::string column = "";
                std::vector< Rule > rules;
            };

          std::string viewName = "";

          std::vector< DataSeriesId > dataSeriesList; //!< Ordened list of DataSeries ID that compose this view
          std::unordered_map< DataSeriesId, terrama2::core::Filter > filtersPerDataSeries; //!< List of filters by DataSeries ID
          std::unordered_map< DataSeriesId, std::string > stylesPerDataSeries; //!< List of base styles by DataSeries ID.
          std::unordered_map< DataSeriesId, Legend > legendPerDataSeries;

          // Parameters to generate a image
          std::string imageName = "";
          te::map::ImageType imageType = te::map::ImageType(1);
          uint32_t imageResolutionWidth = 0; //!< Width resolution of view in pixels
          uint32_t imageResolutionHeight = 0; //!< Height resolution of view in pixels

          uint32_t srid = 0; //!< SRID to aplly in view
        };

        void makeView(ViewId viewId, std::shared_ptr< terrama2::services::view::core::ViewLogger > logger, std::weak_ptr<DataManager> weakDataManager);

        void drawLayersList(ViewPtr view, std::vector< std::shared_ptr<te::map::MemoryDataSetLayer> > layersList, std::shared_ptr< terrama2::services::view::core::ViewLogger > logger);

      } // end namespace core
    }   // end namespace view
  }     // end namespace services
} // end namespace terrama2

#endif //__TERRAMA2_SERVICES_VIEW_CORE_VIEW_HPP__
