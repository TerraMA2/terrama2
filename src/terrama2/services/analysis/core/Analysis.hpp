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
  \file terrama2/services/analysis/core/Analysis.hpp

  \brief Model class for the analysis configuration.

  \author Paulo R. M. Oliveira
*/


#ifndef __TERRAMA2_ANALYSIS_CORE_ANALYSIS_HPP__
#define __TERRAMA2_ANALYSIS_CORE_ANALYSIS_HPP__

#include "../../../core/data-model/DataSeries.hpp"

// STL
#include <string>
#include <vector>

namespace terrama2
{
  namespace services
  {
    namespace analysis
    {
      namespace core
      {
        enum AnalysisType
        {
          PCD_TYPE,
          MONITORED_OBJECT_TYPE,
          GRID_TYPE,
          TERRAME_TYPE
        };

        enum ScriptLanguage
        {
          PYTHON,
          LUA
        };

        enum AnalysisDataSeriesType
        {
          DATASERIES_MONITORED_OBJECT_TYPE,
          DATASERIES_GRID_TYPE,
          DATASERIES_PCD_TYPE,
          ADDITIONAL_DATA_TYPE
        };

        struct AnalysisDataSeries
        {
          uint64_t id;
          terrama2::core::DataSeriesPtr dataSeries;
          AnalysisDataSeriesType type;
          std::string alias;
          std::map<std::string, std::string> metadata;
        };

        struct Analysis
        {
          uint64_t id;
          uint64_t projectId;
					ScriptLanguage scriptLanguage;
					std::string script;
          AnalysisType type;
					std::string name;
					std::string description;
          terrama2::core::DataSet outputDataset;
          std::map<std::string, std::string> metadata;
          std::vector<AnalysisDataSeries> analysisDataSeriesList;
        };

      } // end namespace core
    }   // end namespace analysis
  }     // end namespace services
}       // end namespace terrama2

#endif //__TERRAMA2_ANALYSIS_CORE_ANALYSIS_HPP__
