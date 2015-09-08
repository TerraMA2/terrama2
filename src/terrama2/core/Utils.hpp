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
  \file terrama2/core/Utils.hpp

  \brief Utility functions for TerraMA2 SDK.

  \author Gilberto Ribeiro de Queiroz
*/

#ifndef __TERRAMA2_CORE_UTILS_HPP__
#define __TERRAMA2_CORE_UTILS_HPP__

// STL
#include <string>

namespace terrama2
{
  namespace core
  {
    /*!
      \brief Returns the path relative to a directory or file in the context of TerraMA2.

      \param p A path to be searched in the TerraMA2 context.

      \return A complete path to the file or directory if it is found, otherwise returns an empty string.
     */
    std::string FindInTerraMA2Path(const std::string& p);

  } // end namespace core
}   // end namespace terrama2

#endif // __TERRAMA2_CORE_UTILS_HPP__

