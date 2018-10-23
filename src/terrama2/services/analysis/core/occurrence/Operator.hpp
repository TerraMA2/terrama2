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
  \file terrama2/services/analysis/core/occurrence/Operator.hpp

  \brief Contains occurrence analysis operators (summarization).

  \author Raphael Willian da Costa
*/


#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_OCCURRENCE_OPERATOR_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_OCCURRENCE_OPERATOR_HPP__


// TerraMA2
#include "../Config.hpp"

// STL
#include <string>

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        namespace occurrence
        {
          /*!
            \brief Calculates the count of occurrences in the monitored object.

            This operation does not use Geometry as filter.

            \param dataSeriesName DataSeries name.
            \param dateFilter Time filter for the data.
            \param monitoredIdentifier Monitored Identifier Attribute to join
            \param additionalIdentifier Additional Indentifiere Attribute to join
            \param restriction SQL restriction.

            \return The number of occurrences in the monitored object.
          */
          TMANALYSISEXPORT int count(const std::string& dataSeriesName,
                                     const std::string& dateFilter,
                                     const std::string& monitoredIdentifier,
                                     const std::string& additionalIdentifier,
                                     const std::string& restriction= "");
        }
      }
    }
  }
}

#endif // __TERRAMA2_SERVICES_ANALYSIS_CORE_OCCURRENCE_OPERATOR_HPP__
