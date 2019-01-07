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
#include "Config.hpp"
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

        TMVIEWEXPORT void registerFactories();


        TMVIEWEXPORT te::se::Symbolizer* getSymbolizer(const te::gm::GeomType& geomType,
                                          const std::string& color,
                                          const std::string& opacity) noexcept;

        TMVIEWEXPORT te::se::Stroke* CreateStroke(const std::string& color,
                                     const std::string& width,
                                     const std::string& opacity,
                                     const std::string& dasharray,
                                     const std::string& linecap,
                                     const std::string& linejoin);

        TMVIEWEXPORT te::se::Fill* CreateFill(const std::string& color,
                                 const std::string& opacity);

        /*!
         * \brief Retrieves a string representation of double value with pre-defined precision
         * \param value - Value to cast
         * \param precision - Decimal precision
         * \return String value
         */
        TMVIEWEXPORT std::string toString(const double value, const int& precision = 8);

        /*!
         * \brief Removes table from provided URI.
         * \param name Table name
         * \param uri Connection URI
         */
        TMVIEWEXPORT void removeTable(const std::string& name, const te::core::URI& uri);

        /*!
         * \brief Tries to remove folder
         *
         * \throws Exception when could not remove folder
         *
         * \param folderpath Path to create
         */
        TMVIEWEXPORT void removeFolder(const std::string& folderpath);

        /*!
         * \brief Tries to create folder
         * \param folderpath Path to create
         */
        TMVIEWEXPORT void createFolder(const std::string& folderpath);

        /*!
         * \brief Tries to remove and then create new folder
         * \throws Exception when could not create or remove folder
         *
         * \param folderpath Path to re-criate
         */
        TMVIEWEXPORT void recreateFolder(const std::string& folderpath);

        /*! \brief Tries to remove file from disk
         * \note It only throw exception when the filepath exists but could not remove (Permission management/Lock)
         *
         * \throws Exception when could not remove file.
         *
         * \param filepath Path to file
         */
        TMVIEWEXPORT void removeFile(const std::string& filepath);

        /*!
         * \brief This method aims to clean up extra slashes on URI object
         *
         * \todo This method should be part of te::core::URI implementation. When implemented, it must be deprecated
         *
         * \param uri URI string
         * \return Clean URI object
         */
        TMVIEWEXPORT te::core::URI normalizeURI(const std::string& uri);

      } // end namespace core
    }   // end namespace view
  }     // end namespace services
}       // end namespace terrama2

#endif // __TERRAMA2_SERVICES_VIEW_CORE_UTILS_HPP__
