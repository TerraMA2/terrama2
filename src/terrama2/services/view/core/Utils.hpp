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

  \brief Utility functions for view module.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_SERVICES_VIEW_CORE_UTILS_HPP__
#define __TERRAMA2_SERVICES_VIEW_CORE_UTILS_HPP__

#include <memory>
#include "../../../core/Shared.hpp"
#include "../../../core/data-model/Filter.hpp"

namespace terrama2
{
  namespace services
  {
    namespace view
    {
      namespace core
      {
        void createGeoserverTempMosaic(terrama2::core::DataManagerPtr dataManager, terrama2::core::DataSetPtr dataset, const terrama2::core::Filter& filter, const std::string& exhibitionName, const std::string& outputFolder);

        void createGeoserverPropertiesFile(const std::string& outputFolder, const std::string& exhibitionName, DataSeriesId dataSeriesId);

      } // end namespace core
    }   // end namespace view
  }     // end namespace services
}       // end namespace terrama2

#endif // __TERRAMA2_SERVICES_VIEW_CORE_UTILS_HPP__
