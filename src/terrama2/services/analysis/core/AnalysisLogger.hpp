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
  \file terrama2/services/analysis/core/AnalysisLogger.hpp

  \brief Class to log the steps of Terrama2 Analysis Service

  \author Vinicius Campanha
*/

#ifndef __TERRAMA2_SERVICES_ANALYSIS_CORE_ANALYSISLOG_HPP__
#define __TERRAMA2_SERVICES_ANALYSIS_CORE_ANALYSISLOG_HPP__

// TerraMa2
#include "Config.hpp"
#include "Typedef.hpp"
#include "../../../core/utility/ProcessLogger.hpp"

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        /*!
         * \brief The AnalysisLogger class is responsible for uses the Process Logger to
         * log the processes in the Analysis Service.
         */
        class TMANALYSISEXPORT AnalysisLogger : public terrama2::core::ProcessLogger
        {
        public:

          /*!
           * \brief Class constructor, it will pass the connection information about the Log DB and
           * set the analysis log table.
           * \param connInfo Has the access information to the log Database
           */
          AnalysisLogger();

          /*!
           * \brief Class destructor
           */
          virtual ~AnalysisLogger() = default;

          virtual void setConnectionInfo(const te::core::URI& uri) override;

          virtual std::shared_ptr<ProcessLogger> clone() const override;

        };
      }
    }
  }
}
#endif // __TERRAMA2_SERVICES_ANALYSIS_CORE_ANALYSISLOG_HPP__
