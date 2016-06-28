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
  \file terrama2/core/data-model/DataSeriesRisk.hpp

  \brief Models the information of a DataSeriesRisk.

  \author Jano Simas
*/

#ifndef __TERRAMA2_CORE_DATA_MODEL_DATA_SERIES_RISK_HPP__
#define __TERRAMA2_CORE_DATA_MODEL_DATA_SERIES_RISK_HPP__

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
      uint32_t level = 0; //!< Level of the risk, should be unique in a DataSeriesRisk.
      bool hasLowerBound = false;
      double lowerBound = 0; //!< Lower bound for real values, inclusive.
      bool hasUpperBound = false;
      double upperBound = 0; //!< Upper bound for real values, exclusive.
      std::string textValue; //!< Text value for this RiskLevel.

      //! Minor operator for sorting.
      bool operator<(const RiskLevel& rhs) const { return level < rhs.level; }
    };

    enum class RiskType
    {
      NUMERIC = 1,
      TEXT = 2
    };

    /*!
      \brief Risk information of an alert
    */
    struct DataSeriesRisk
    {
      DataSeriesRiskId id = 0; //!< The identifier of the DataSeriesRisk.
      DataSeriesId dataSeriesId = 0; //!< The DataSeries that will be used for risk analysis.
      std::string name; //!< Name of the DataSeriesRisk.
      std::string description; //!< Short description of the purpose of the DataSeriesRisk.
      RiskType riskType = RiskType::NUMERIC;

      std::string attribute;//!< Attribute of the DataSeries that will be used for risk analysis.

      /*!
        \brief Compute risk level for the valeu
      */
      uint32_t riskLevel(const std::string& value) const;
      /*!
        \brief Compute risk level for the valeu
      */
      uint32_t riskLevel(double value) const;

      std::vector<RiskLevel> riskLevels;//!< List of risk levels of the DataSeriesRisk.
    };
  } // end namespace core
} // end namespace terrama2

#endif // __TERRAMA2_CORE_DATA_MODEL_DATA_SERIES_RISK_HPP__
