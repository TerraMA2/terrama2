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
  \file terrama2/core/core.hpp

  \brief This file contains include headers for the TerraMA2 core module.

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_HPP__
#define __TERRAMA2_CORE_HPP__

// TerraMA2
#include "data-access/DataAccessor.hpp"
#include "data-access/DataStorager.hpp"
#include "data-access/DataRetriever.hpp"
#include "data-access/Series.hpp"


#include "data-model/DataManager.hpp"
#include "data-model/DataProvider.hpp"
#include "data-model/DataSeries.hpp"
#include "data-model/DataSet.hpp"
#include "data-model/Filter.hpp"
#include "data-model/Schedule.hpp"

#include "network/TcpManager.hpp"

#include "utility/DataAccessorFactory.hpp"
#include "utility/DataStoragerFactory.hpp"
#include "utility/DataRetrieverFactory.hpp"
#include "utility/SemanticsManager.hpp"
#include "utility/ServiceManager.hpp"
#include "utility/Version.hpp"


namespace terrama2
{
  /*! \brief Namespace for the TerraMA2 core module, it contains the base classes for the TerraMA2. */
  namespace core
  {
  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_HPP__
