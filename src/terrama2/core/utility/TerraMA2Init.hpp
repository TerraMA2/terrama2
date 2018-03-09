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
  \file terrama2/core/utility/TerraMA2Init.hpp
  \brief A Raii initialization class for TerraMA2

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_TERRAMA2_INIT_HPP__
#define __TERRAMA2_CORE_TERRAMA2_INIT_HPP__

// TerraMa2
#include "../Config.hpp"

// STL
#include <string>

namespace terrama2
{
  namespace core
  {
    class TMCOREEXPORT TerraMA2Init
    {
      public:
        TerraMA2Init(const std::string& serviceType, const int listeningPort);
        ~TerraMA2Init();

        TerraMA2Init(const TerraMA2Init& other) = delete;
        TerraMA2Init(TerraMA2Init&& other) = delete;
        TerraMA2Init& operator=(const TerraMA2Init& other) = delete;
        TerraMA2Init& operator=(TerraMA2Init&& other) = delete;
    };
  } /* core */
} /* terrama2 */

#endif //__TERRAMA2_CORE_TERRAMA2_INIT_HPP__
