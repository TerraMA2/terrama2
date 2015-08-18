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
  \file terrama2/core/Version.hpp

  \brief Utility class for system versioning.

  \author Gilberto Ribeiro de Queiroz
*/

#ifndef __TERRAMA2_INTERNAL_CORE_VERSION_HPP__
#define __TERRAMA2_INTERNAL_CORE_VERSION_HPP__

// STL
#include <string>

namespace terrama2
{
  namespace core
  {
    //! Utility class for system versioning.
    class TMA_CORE_EXPORT Version
    {
      public:

        static int majorNumber();

        static int minorNumber();

        static int patchNumber();

        static std::string releaseStatus();

        static std::string buildDate();

        static std::string asString();

        static int asInt();

      private:

        Version();

        ~Version();
    };

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_INTERNAL_CORE_VERSION_HPP__

