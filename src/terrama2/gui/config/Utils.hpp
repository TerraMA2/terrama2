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
  \file terrama2/gui/config/Utils.hpp

  \brief Helpers for TerraMA2 GUI Configuration module.

  \author Raphael Willian da Costa

*/

#ifndef __TERRAMA2_GUI_CONFIG_UTILS_HPP__
#define __TERRAMA2_GUI_CONFIG_UTILS_HPP__

#include <stdint.h>

// Forward declaration
class QString;

namespace terrama2
{
  // Forward declaration
  namespace core
  {
    class DataSet;
    class DataSetItem;
  }

  namespace gui
  {
    namespace config
    {
      // Forward declaration
      class ConfigApp;

      /*!
        \brief It is a common way to save terrama2::core::DataSet.

        It sends dataset via gsoap client and log it in TerraMA2 log file through terrama2::core::Logger.

        \param dataset A terrama2::core::DataSet reference to be saved.
        \param datasetItem A terrama2::core::DataSetItem reference used to set in dataset
        \param provider A uint64_t value containing terrama2::core::DataProvider id.
        \param app A pointer to TerraMA2 Main Window.
        \param selectedName A string value for handling active dataset selected inside tab.
        \param inputValue A string value containing input dataset name.
        \param iconName A string value to be used in QTreeWidgetItem icon.

        \exception terrama2::Exception If it is not possible to update the data provider.

        \note The log should be initialized before. See more in terrama2::core::Logger
      */
      void saveDataSet(terrama2::core::DataSet& dataset, terrama2::core::DataSetItem& datasetItem, const uint64_t provider, ConfigApp* app,
                       const QString& selectedName, const QString& inputValue, const QString& iconName);
    }
  }
}

#endif // __TERRAMA2_GUI_CONFIG_UTILS_HPP__
