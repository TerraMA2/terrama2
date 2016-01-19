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
  \file terrama2/collector/collector.hpp

  \brief This file contains include headers for the TerraMA2 collector module.

  \author Jano Simas
*/

#ifndef __TERRAMA2_COLLECTOR_COLLECTOR_HPP__
#define __TERRAMA2_COLLECTOR_COLLECTOR_HPP__

// TerraMA2
#include "CollectorService.hpp"
#include "DataFilter.hpp"
#include "DataRetriever.hpp"
#include "DataRetrieverFTP.hpp"
#include "DataSetTimer.hpp"
#include "Exception.hpp"
#include "Factory.hpp"
#include "IntersectionOperation.hpp"
#include "Log.hpp"
#include "Parser.hpp"
#include "ParserFirePoint.hpp"
#include "ParserOGR.hpp"
#include "ParserPcdInpe.hpp"
#include "ParserPcdToa5.hpp"
#include "ParserPostgis.hpp"
#include "Storager.hpp"
#include "StoragerPostgis.hpp"
#include "Utils.hpp"



namespace terrama2
{
  /*!
     \brief Namespace for the TerraMA2 collector module

     It contains the base classes to retrive and store data.

     \note Be aware to initialize terrama2::core::Logger before to display status message in runtime collector module.
 */
  namespace collector
  {
  } // end namespace colelctor
}   // end namespace terrama2

#endif  // __TERRAMA2_COLLECTOR_COLLECTOR_HPP__
