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
  \file terrama2/core/data-model/Risk.hpp

  \brief Models the information of a Risk.

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_DATA_MODEL_RISK_HPP__
#define __TERRAMA2_CORE_DATA_MODEL_RISK_HPP__

#include "DataSet.hpp"
#include "../Shared.hpp"

#include <vector>
#include <algorithm>

namespace terrama2
{
  namespace core
  {
    struct RiskLevel
    {
      std::string name;
      uint32_t level = 0; //!< Level of the risk, should be unique in a Risk.
      double value = 0; //!< Numeric value for the risk level..

      //! Minor operator for sorting.
      bool operator<(const RiskLevel& rhs) const { return level < rhs.level; }
    };

    /*!
      \brief Risk information of an alert
    */
    struct Risk
    {
      std::string name; //!< Name of the Risk.
      std::string description; //!< Short description of the purpose of the Risk.
      std::vector<RiskLevel> riskLevels;//!< List of risk levels of the Risk.

      /*!
        \brief Compute risk level for the value

        \return A tuple with risk level and level name
      */
      std::tuple<int, std::string>  riskLevel(double value) const;

      /*!
       * \brief Returns the risk name for the level
       * \param level The level
       * \return
       */
      std::string riskName(const int level) const;

    };
  } // end namespace core
} // end namespace terrama2

#endif // __TERRAMA2_CORE_DATA_MODEL_RISK_HPP__
