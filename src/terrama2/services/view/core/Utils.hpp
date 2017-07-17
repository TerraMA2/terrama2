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
  \file terrama2/services/view/core/View.hpp

  \brief Utility functions for view module.

  \author Paulo R. M. Oliveira
*/

#ifndef __TERRAMA2_SERVICES_VIEW_CORE_UTILS_HPP__
#define __TERRAMA2_SERVICES_VIEW_CORE_UTILS_HPP__

// TerraMA2
#include "../../../core/Shared.hpp"
#include "../../../core/data-model/Filter.hpp"
#include "../../../core/utility/FileRemover.hpp"
#include "../../../impl/DataAccessorFile.hpp"

// TerraLib
#include <terralib/se/Symbolizer.h>
#include <terralib/core/uri.h>

// STD
#include <memory>

namespace terrama2
{
  namespace services
  {
    namespace view
    {
      namespace core
      {

        void registerFactories();


        te::se::Symbolizer* getSymbolizer(const te::gm::GeomType& geomType,
                                          const std::string& color,
                                          const std::string& opacity) noexcept;

        te::se::Stroke* CreateStroke(const std::string& color,
                                     const std::string& width,
                                     const std::string& opacity,
                                     const std::string& dasharray,
                                     const std::string& linecap,
                                     const std::string& linejoin);

        te::se::Fill* CreateFill(const std::string& color,
                                 const std::string& opacity);

        /*!
         * \brief Retrieves a string representation of double value with pre-defined precision
         * \param value - Value to cast
         * \param precision - Decimal precision
         * \return String value
         */
        std::string toString(const double value, const int& precision = 8);

        /*!
         * \brief Removes table from provided URI.
         * \param name Table name
         * \param uri Connection URI
         */
        void removeTable(const std::string& name, const te::core::URI& uri);

        /*!
         * \brief Tries to remove file from disk
         *
         * \note It does not throw exception if files does not exist.
         *
         * \throws Exception when could not remove file.
         *
         * \param filepath Path to file
         */
        void removeFile(const std::string& filepath);

      } // end namespace core
    }   // end namespace view
  }     // end namespace services
}       // end namespace terrama2

#endif // __TERRAMA2_SERVICES_VIEW_CORE_UTILS_HPP__
