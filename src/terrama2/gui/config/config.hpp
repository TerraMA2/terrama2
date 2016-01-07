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
  \file terrama2/gui/config/configuration.hpp

  \brief This file contains include headers for the TerraMA2 GUI config module.

  \author Gilberto Ribeiro de Queiroz
 */

#ifndef __TERRAMA2_GUI_CONFIG_CONFIGURATION_HPP__
#define __TERRAMA2_GUI_CONFIG_CONFIGURATION_HPP__

// TerraMA2 includes
#include "ConfigApp.hpp"
#include "ConfigAppTab.hpp"
#include "ConfigAppWeatherGrid.hpp"
#include "ConfigAppWeatherOccurrence.hpp"
#include "ConfigAppWeatherPcd.hpp"
#include "ConfigAppWeatherServer.hpp"
#include "ConfigAppWeatherTab.hpp"
#include "Exception.hpp"
#include "FilterDialog.hpp"
#include "IntersectionDialog.hpp"
#include "LuaSyntaxHighlighter.hpp"
#include "PcdDialog.hpp"
#include "SurfaceDialog.hpp"
#include "CollectorRuleDialog.hpp"


namespace terrama2
{
  /*!
    \brief A namespace for handling TerraMA2 gui applications
  */
  namespace gui
  {
    /*!
      \brief A namespace for handling TerraMA2 configuration module
    */
    namespace config
    {
    }
  }
}

#endif // __TERRAMA2_GUI_CONFIG_CONFIGURATION_HPP__
