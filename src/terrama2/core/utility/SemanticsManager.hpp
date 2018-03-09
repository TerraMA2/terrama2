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
  \file terrama2/core/data-model/SemanticsManager.hpp

  \brief

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_DATA_MODEL_SEMANTICS_MANAGER_HPP__
#define __TERRAMA2_CORE_DATA_MODEL_SEMANTICS_MANAGER_HPP__

// TerraMa2
#include "../Config.hpp"
#include "../data-model/DataSeriesSemantics.hpp"
#include "../data-model/DataProvider.hpp"

// STL
#include <string>
#include <unordered_map>

// TerraLib
#include <terralib/common/Singleton.h>

namespace terrama2
{
  namespace core
  {
    class TMCOREEXPORT SemanticsManager : public te::common::Singleton<SemanticsManager>
    {
      public:
        /*!
          \brief Register a new DataSeriesSemantics

          \exception terrama2::core::SemanticsException raised when a semantics with same name is already present
        */
        DataSeriesSemantics addSemantics(const std::string& code,
                                         const std::string& name,
                                         const std::string& driver,
                                         const DataSeriesType& dataSeriesType,
                                         const DataSeriesTemporality& dataSeriesTemporality,
                                         const DataFormat& format,
                                         const std::vector<DataProviderType>& providersTypeList,
                                         const std::unordered_map<std::string, std::string>& metadata);
        /*!
          \brief Recover a DataSeriesSemantics by name

          \exception terrama2::core::SemanticsException raised when the semantics is not defined
        */
        DataSeriesSemantics getSemantics(const std::string& semanticsCode);

      protected:
        friend class te::common::Singleton<SemanticsManager>;
        //! Default constructor
        SemanticsManager() {}
        //! Default destructor
        virtual ~SemanticsManager() {}

        SemanticsManager(const SemanticsManager& other) = delete;
        SemanticsManager(SemanticsManager&& other) = delete;
        SemanticsManager& operator=(const SemanticsManager& other) = delete;
        SemanticsManager& operator=(SemanticsManager&& other) = delete;

      private:
        std::unordered_map<std::string, DataSeriesSemantics> semanticsMap_;//!< Semantics map by code
    };
  } /* core */
} /* terrama2 */

#endif //__TERRAMA2_CORE_DATA_MODEL_SEMANTICS_MANAGER_HPP__
