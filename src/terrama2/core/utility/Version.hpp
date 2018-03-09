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

#ifndef __TERRAMA2_CORE_VERSION_HPP__
#define __TERRAMA2_CORE_VERSION_HPP__

// TerraMa2
#include "../Config.hpp"

// STL
#include <string>

namespace terrama2
{
  namespace core
  {
    //! Utility class for system versioning.
    class TMCOREEXPORT Version
    {
      public:

        //! Return the major version of TerraMA2.
        static int majorNumber();

        //! Return the minor revision of TerraMA2.
        static int minorNumber();

        //! Return the patch revision number of TerraMA2.
        static int patchNumber();

        //! Tells the build date.
        static std::string buildDate();

        //! Returns the version as a string to be read by humans.
        static std::string asString();

        //! Returns the version as a integer number.
        static int asInt();

      private:

        //! Not constructible.
        Version();

        //! Not constructible.
        ~Version();
    };

  } // end namespace core
}   // end namespace terrama2

#endif  // __TERRAMA2_CORE_VERSION_HPP__
