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

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_CORE_CORE_HPP__
#define __TERRAMA2_CORE_CORE_HPP__

// TerraMA2
#include "ApplicationController.hpp"
#include "DataManager.hpp"
#include "DataProvider.hpp"
#include "DataSet.hpp"
#include "DataSetItem.hpp"
#include "Exception.hpp"
#include "Filter.hpp"
#include "Intersection.hpp"
#include "Logger.hpp"
#include "PCD.hpp"
#include "PCDAttribute.hpp"
#include "PCDDataSet.hpp"
#include "Utils.hpp"
#include "Version.hpp"


namespace terrama2
{
  /*! \brief Namespace for the TerraMA2 core module, it contains the base classes for the TerraMA2. */
  namespace core
  {
  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_CORE_HPP__