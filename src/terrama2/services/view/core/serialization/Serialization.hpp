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
  \file terrama2/services/view/core/serialization/Serialization.hpp

  \brief Serialization methods for View service components

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_VIEW_CORE_SE_SERIALIZATION_HPP__
#define __TERRAMA2_SERVICES_VIEW_CORE_SE_SERIALIZATION_HPP__

//TerraMA2
#include "../View.hpp"

// Terralib
#include <terralib/se/Style.h>

// Std
#include <memory>

namespace terrama2
{
  namespace services
  {
    namespace view
    {
      namespace core
      {
          class Serialization
          {
            public:
              /*!
               *\brief Creates a style XML file compatible with GeoServer
               */
              static void writeVectorialStyleGeoserverXML(const te::se::Style* style, const std::string path);

              /*!
               * \brief Method to read a Style from a xml file and return a TerraLib Style object
               * \param path Fullpath to the XML file
               * \return A unique_ptr to a TerraLib Style object
               */
              static std::unique_ptr<te::se::Style> readVectorialStyleXML(const std::string path);

              static void writeCoverageStyleGeoserverXML(const View::Legend legend,
                                                         const std::string path);

          };
      } // end namespace core
    }   // end namespace view
  }     // end namespace services
}       // end namespace terrama2

#endif // __TERRAMA2_SERVICES_VIEW_CORE_SE_SERIALIZATION_HPP__
