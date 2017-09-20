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
  \file terrama2/services/analysis/core/utility/Verify.hpp

  \brief Utility functions for easy consistency check

  \author Jano Simas
*/


#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_VERIFY_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_VERIFY_HPP__

#include "../Shared.hpp"
#include "../Analysis.hpp"

// STL
#include <set>

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        namespace verify
        {
          void analysisType(const terrama2::services::analysis::core::AnalysisPtr analysis, int analysisType);
          void analysisGrid(const terrama2::services::analysis::core::AnalysisPtr analysis);
          void analysisMonitoredObject(const terrama2::services::analysis::core::AnalysisPtr analysis);
          void analysisDCP(const terrama2::services::analysis::core::AnalysisPtr analysis);


          /*!
            \brief Verifies if the given analysis in using an inactive data series.
            \note In case there is a inactive data series, this information must be logged but the analysis will be executed normally.
            \param dataManager A smart pointer to the data manager.
            \param analysis The analysis to be executed.
            \return A list with error messages.
          */
          std::set<std::string> inactiveDataSeries(DataManagerPtr dataManager, AnalysisPtr analysis);


          /*!
            \brief Validates an analysis to check if all configurations are set correctly and if there is data available in the data series.
            \param dataManager A smart pointer to the data manager.
            \param analysis The analysis to be executed.
            \param validateResult The validate result object.
          */
          void validateAnalysis(DataManagerPtr dataManager, AnalysisPtr analysis, ValidateResult& validateResult);

          /*!
            \brief Verifies if all data series used in the analysis have data available.
            \param dataManager A smart pointer to the data manager.
            \param analysis The analysis to be executed.
            \return A list with error messages.
          */
          std::set<std::string> dataAvailable(DataManagerPtr dataManager, AnalysisPtr analysis);

          /*!
            \brief Verifies if the monitored object data series contains the identifier attribute
            \param dataManager A smart pointer to the data manager.
            \param analysis The analysis to be executed.
            \return A list with error messages.
          */
          std::set<std::string> attributeIdentifier(DataManagerPtr dataManager, AnalysisPtr analysis);


        } /* verify */
      } /* core */
    } /* analysis */
  } /* services */
} /* terrama2 */

#endif //__TERRAMA2_SERVICES_ANALYSIS_CORE_VERIFY_HPP__
